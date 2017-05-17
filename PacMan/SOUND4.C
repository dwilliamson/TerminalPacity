/*
A.dvanced                        ADAIS
D.igital                         -----
A.udio
I.nterface
S.ystem

Copyright (c) 1997 Don Williamson
*/

#include "..\library\ems.c"

unsigned int DSP_Reset;
unsigned int DSP_Read_Data;
unsigned int DSP_Write_Data;
unsigned int DSP_Write_Status;
unsigned int DSP_Data_Avail;
unsigned int DSP_Port;

unsigned int SB_Reset = 0;

unsigned char DMA_Channel    = 3;
unsigned int  DMA_Page[4]    = { 0x87, 0x83, 0x81, 0x82 };
unsigned char DMA_Address[4] = { 0, 2, 4, 6 };
unsigned char DMA_Length[4]  = { 1, 3, 5, 7 };

unsigned char IRQ_Jumper      = 10;
unsigned char IRQ_Number[11]  = { 0x00, 0x00, 0x0A, 0x0B, 0x00, 0x0D,
                                  0x00, 0x0F, 0x00, 0x00, 0x72};

unsigned char        SB_Sample_Playing = 0;
static unsigned char SB_Old_Mask       = -1;
static unsigned char SB_Mask_Bit       = -1;

unsigned char SB_Next_Bool = 0;
unsigned int  SB_Played    = 0;
unsigned int  SB_Next_Len  = 0;
unsigned char far *SB_Next_Ptr;

byte *pf_addr;

typedef struct sample_typ
{
   unsigned int       length;
   unsigned char far *data;
} sample, *sample_ptr;

typedef struct emm_sample_typ
{
   unsigned int  handle;
   unsigned int  length;
   unsigned int  pages;
} emsample, *emsample_ptr;

void (_interrupt far *Old_IRQ_Isr)();
unsigned char Init_EMM(void);

void New_Delay(unsigned long clocks)
{
   unsigned long elapsed = 0;
   unsigned int last, next, ncopy;

   outp(0x43, 0);                              
   last = inp(0x40);                            
   last =~ ((inp(0x40) << 8) + last);            

   do
   {
      outp(0x43,0);                           
      next = inp(0x40);                         
      ncopy = next =~ ((inp(0x40) << 8) + next);   

      next -= last;      

      elapsed += next; 
      last = ncopy;
   }
   while (elapsed < clocks);
}

unsigned char Get_SB_Env(void)
{
   char *str;
   int ret;

   str = getenv("BLASTER");
   if (!str) return(0);
   ret = sscanf(str, "A%i I%i D%i", &DSP_Port, &IRQ_Jumper, &DMA_Channel);
   if (ret != 3 || IRQ_Jumper < 2 || IRQ_Jumper > 10 || DMA_Channel > 3)
      return(0);
   return(1);
}

unsigned int Reset_SoundBlaster(port)
{
   DSP_Reset        = port + 0x06;
   DSP_Read_Data    = port + 0x0A;
   DSP_Write_Data   = port + 0x0C;
   DSP_Write_Status = port + 0x0C;
   DSP_Data_Avail   = port + 0x0E;
   outp(DSP_Reset, 1);
   delay(10);
   outp(DSP_Reset, 0);
   while (!(inp(DSP_Data_Avail) & 0x80));
   delay(10);
   if (inp(DSP_Read_Data) == 0xAA) return(1);
   else return(0);
}

void Speaker_On(void)
{
   while ((inp(DSP_Write_Data) & 0x80));
   outp(DSP_Write_Data, 0xD1);
}

void Speaker_Off(void)
{
   while ((inp(DSP_Write_Data) & 0x80));
   outp(DSP_Write_Data, 0xD3);
}

void DSP_Write(unsigned char value)
{
   while ((inp(DSP_Write_Data) & 0x80));
   outp(DSP_Write_Data, value);
}

unsigned char DSP_Read(unsigned char value)
{
   while (!(inp(DSP_Data_Avail) & 0x80));
   return (inp(DSP_Read_Data));
}

void PCM_Load(sample_ptr sample, char *filename, unsigned int length)
{
   FILE *fp;
   unsigned int x = 0;

   if ((fp = fopen(filename, "rb")) == NULL)
   {
      printf("\nError opening file.");
      exit(1);
   }
   while (x < length)
   {
      sample->data[x] = fgetc(fp);
      x++;
   }
   fclose(fp);
   sample->length = length--;
}

void PCM_Load_EMS(emsample_ptr sample, char *filename, unsigned int length)
{
   FILE *fp;
   unsigned int x = 0;

   if (length <= 16384) sample->pages = 1;
   if (length <= 32768 && length > 16384) sample->pages = 2;
   if (length <= 49152 && length > 32768) sample->pages = 3;
   if (length > 49152) sample->pages = 4;
   for (x = 0; x < sample->pages; x++)
   {
      EMM_Map_Memory(sample->handle, (char)x, x);
   }
//   EMM_Multiple_Map(sample->handle, 0, 0, sample->pages);

   if ((fp = fopen(filename, "rb")) == NULL)
   {
      printf("\nError opening file.");
      exit(1);
   }

   while (x < length)
   {
      pf_addr[x] = fgetc(fp);
      x++;
   }
   fclose(fp);
   sample->length = length--;
}

unsigned int Play_Sample_DMA(unsigned char *sample, unsigned int length)
{
   unsigned long offset;
   unsigned page;

   offset = ((FP_SEG(sample) & 0xFFF) << 4) + FP_OFF(sample) + 1;
   page   = FP_SEG(sample) >> 12;

//   if (offset >= 0x10000)
//   {
//     page++;
//      offset -= 0x10000;
//   }
//   if (offset + length > 0x10000)
//   {
//      printf(" ---=== Cut! ===---\n");
//      length = 0x10000 - offset;
//   }
   length--;

   outp(0x0A, (4 + DMA_Channel));
   outp(0x0C, 0);
   outp(0x0B, (0x48 + DMA_Channel));
   outp(DMA_Address[DMA_Channel], (offset & 255));
   outp(DMA_Address[DMA_Channel], ((offset & 65280) >> 8));
   outp(DMA_Page[DMA_Channel], page);
   outp(DMA_Length[DMA_Channel], (length & 255));
   outp(DMA_Length[DMA_Channel], ((length & 65280) >> 8));
   outp(0x0A, DMA_Channel);

   DSP_Write(0x14);
   DSP_Write((length & 255));
   DSP_Write(((length & 65280) >> 8));
   length++;
   return(length);
}

void SBPlay(sample_ptr sample, unsigned int length)
{
   Stop_Sample();
   SB_Played = Play_Sample_DMA(sample->data, length);
   if (SB_Played < length)
   {
      SB_Next_Len = length - SB_Played;
      SB_Next_Bool = 1;
   }
   if (IRQ_Jumper != 10)
   {
      SB_Old_Mask = inp(0x21);
      outp(0x21, SB_Old_Mask & ~(1 << IRQ_Jumper));
   }
   if (IRQ_Jumper == 10)
   {
      SB_Old_Mask = inp(0xA1);
      outp(0xA1, SB_Old_Mask & ~(1 << 2));
   }
   DSP_Write(0xD4);
   SB_Sample_Playing = 1;
}

void SBPlayEMS(emsample_ptr sample, unsigned int length)
{
   unsigned int x;

   EMM_Restore_State(sample->handle);
   Stop_Sample();
   EMM_Save_State(sample->handle);
   for (x = 0; x < sample->pages; x++)
   {
      EMM_Map_Memory(sample->handle, (char)x, x);
   }
//   EMM_Multiple_Map(sample->handle, 0, 0, sample->pages);

   SB_Played = Play_Sample_DMA(pf_addr, length);
   if (IRQ_Jumper != 10)
   {
      SB_Old_Mask = inp(0x21);
      outp(0x21, SB_Old_Mask & ~(1 << IRQ_Jumper));
   }
   if (IRQ_Jumper == 10)
   {
      SB_Old_Mask = inp(0xA1);
      outp(0xA1, SB_Old_Mask & ~(1 << 2));
   }
   DSP_Write(0xD4);
   SB_Sample_Playing = 1;
}

void Play_Sample_DAC(sample_ptr sample, unsigned int length)
{
   int x;

   for (x = 0; x < length; x++)
   {
      DSP_Write(0x10);
      DSP_Write(sample->data[x]);
      New_Delay(80);
   }
   printf("%lu", &sample);
}

void _interrupt far DMA_Acknowledge(void)
{
   unsigned int temp;

   Stop_Sample();
   temp = inp(DSP_Data_Avail);
   outp(0x20, 0x20);
   if (IRQ_Jumper == 10) outp(0xA0, 0x20);
   SB_Next_Bool = 0;
   spi[0] = spi[1] = 0;
}

void Stop_Sample(void)
{
   if (SB_Sample_Playing)
   {
      SB_Sample_Playing = 0;
      DSP_Write(0xD0);

      if (IRQ_Jumper != 10)
      {
         SB_Mask_Bit = inp(0x21);
         if (SB_Old_Mask & (1 << IRQ_Jumper)) SB_Mask_Bit |= (1 << IRQ_Jumper);
         else SB_Mask_Bit &= ~(1 << IRQ_Jumper);
         outp(0x21, SB_Mask_Bit);
      }
      if (IRQ_Jumper == 10)
      {
         SB_Mask_Bit = inp(0xA1);
         if (SB_Old_Mask & (1 << 2)) SB_Mask_Bit |= (1 << 2);
         else SB_Mask_Bit &= ~(1 << 2);
         outp(0xA1, SB_Mask_Bit);
      }
   }
   spi[0] = spi[1] = 0;
}

void Init_Blaster(unsigned int frequency)
{
   int x;

   for (x = 0x210; x < 0x270; x+= 0x10)
   {
      if ((SB_Reset = Reset_SoundBlaster(x)))
      {
         if (!Get_SB_Env())
         {
            SB_Reset = 0;
            goto No_SB;
         }
         if (!Init_EMM())
         {
            SB_Reset = 0;
            goto No_SB;
         }

         // Setup Interrupt Service Routine for acknowledging DMA transfer

         Old_IRQ_Isr = _dos_getvect(IRQ_Number[IRQ_Jumper]);
         _dos_setvect(IRQ_Number[IRQ_Jumper], DMA_Acknowledge);

//         outp(DSP_Write_Data, 0xD1); // Turn speaker on
         Speaker_On();

         DSP_Write(0x40);                         // Set Playback Frequency
         DSP_Write(256 - (1000000 / frequency));

         break;
      }
   }
   No_SB:;
}

void Release_Blaster(void)
{
   Stop_Sample();

//   outp(DSP_Write_Data, 0xD3); // Turn speaker off
   Speaker_Off();

   // Restore default Interrupt Service Routine

   _dos_setvect(IRQ_Number[IRQ_Jumper], Old_IRQ_Isr);
}

unsigned char Init_EMM(void)
{
   printf("\nInit_EMM: ");
   if (!EMM_Check_Install())
   {
      printf("Expanded Memory Manager not detected...");
      return(0);
   }
   printf("Expanded Memory Manager detected...");
   if (!EMM_Get_Version())
   {
      EMM_Print_Error();
      return(0);
   }
   EMM_Get_Page_Frame(&pf_addr);
   printf(" Version - %x.%x", EMMVersionMajor, EMMVersionMinor);
   EMM_Get_Pages();
   printf(" Free - %uk", EMMFreePages << 4);
   return(1);
}

