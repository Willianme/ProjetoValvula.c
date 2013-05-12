//PROJETO VALVULA - MAIO/2013

#include<16f877a.h> // Microcontrolador a ser utilizado
#device adc=10 // Tamanho do ADC
#use delay(clock=20000000) // 20MHz, clock do processador
#use rs232(baud = 9600, parity = N, xmit = pin_c6, rcv = pin_c7, bits = 8)
#fuses hs, nolvp, nowdt, noput// Fusiveis de configuração
#define use_portb_lcd TRUE // Bit de config. para ligar o LCD na port B
#include <lcd.c> // Driver de controle DO LCD
#include <stdlib.h>
#define led   pin_c1
#define p 0.11850000

// Definição de variável(is) global(is)

int1 i1_Sentido;
int i8_Passo[8], i8_Modo = 0, i8_ModoDivisao = 1, i8_LimiteK = 0;
int16 i16_Velocidade = 0;
float f_Resolucao = 0, f_Angulo = 0, f_Posicao = 0, f_Linear = 0;
char s_Resolucao[4], s_Posicao[10], s_Velocidade[4], s_Modo[1]; //s_Sentido[1]

// Declaração de funções utilizadas no programa principal

void Excitacao()
{
   switch ( i8_Modo )
   {
   case 0:
      printf("Modo de Excitação em Passo Completo - Fase Unica\n\r");
      i8_Passo[3] = 0b00010000;
      i8_Passo[2] = 0b01000000;
      i8_Passo[1] = 0b00100000;
      i8_Passo[0] = 0b10000000;        
      i8_ModoDivisao = 1;
      i8_LimiteK = 3;        
      break;  
   case 1:
      printf("Modo de Excitação em Passo Completo - Fase Dupla\n\r");
      i8_Passo[3] = 0b01010000;
      i8_Passo[2] = 0b01100000;
      i8_Passo[1] = 0b10100000;
      i8_Passo[0] = 0b10010000;
      i8_ModoDivisao = 1;
      i8_LimiteK = 3;
      break;
   case 2:
      printf("Modo de Excitação em Meio Passo - Fase Unica/Fase Dupla\n\r");
      i8_Passo[7] = 0b00010000;
      i8_Passo[6] = 0b01010000;
      i8_Passo[5] = 0b01000000;
      i8_Passo[4] = 0b01100000;
      i8_Passo[3] = 0b00100000;
      i8_Passo[2] = 0b10100000;
      i8_Passo[1] = 0b10000000;
      i8_Passo[0] = 0b10010000;
      i8_ModoDivisao = 2;
      i8_LimiteK = 7;
      break;
   default:
      printf("Modo de Excitação em Passo Completo <default> - Fase Unica\n\r");
      i8_Passo[3] = 0b00010000;
      i8_Passo[2] = 0b01000000;
      i8_Passo[1] = 0b00100000;
      i8_Passo[0] = 0b10000000;
      i8_ModoDivisao = 1;
      i8_LimiteK = 3;
      break;
   }   
   delay_ms(250);
   return;
}

void Inicializacao()
{
   // Inicialização para controle do motor de passo

   printf("Resolucao do Motor [grau/passo] = \n\r");
   gets(s_Resolucao);               // após <enter>, lê o valor do tipo string digitado
   f_Resolucao = atof(s_Resolucao);         // converte string em número do tipo float
   delay_ms(250);

   printf("Selecione o modo de excitacao das bobinas do motor:\n\r");
   printf("   0 - Passo Completo: Fase Unica;\n\r");
   printf("   1 - Passo Completo: Fase Dupla;\n\r");
   printf("   2 - Meio Passo;\n\r");
   printf("Opcao escolhida: ");
   gets(s_Modo);
   i8_Modo = atoi(s_Modo);               // converte string em número do tipo int
   delay_ms(250);
   Excitacao();

   printf("Digite a velocidade entre passos desejada [ms] 0 - 5000: \n\r");
   gets(s_Velocidade);
   i16_Velocidade = atoi(s_Velocidade);
   delay_ms(250);

   return;
}

void main()
{
   int k = 0;

   int16 val = 0;
   float ang = 0;

   lcd_init (); //Inicializa o LCD
   delay_ms (10) ;
   setup_adc (ADC_CLOCK_INTERNAL) ; //Configura ADC
   setup_adc_ports (AN0) ;
   set_adc_channel (0) ;
   delay_ms (10) ;
   printf (lcd_putc, "\fTec. Mecatronica\n") ; //Menssagens de saudação exibidas no LCD
   delay_ms (1000) ;
   printf (lcd_putc, " *PUC - PARANA*") ;
   delay_ms (1000) ;
   printf (lcd_putc, "\fSaulo | Willian\n") ;
   delay_ms (900) ;
   printf (lcd_putc, "Wilston | Thiago") ;
   delay_ms (900) ;

   val = read_adc (); //Le valor DO ADC
   ang = p * val; //Resolve o angulo para o valor lido
   printf (lcd_putc, "\fValor do Angulo \n  %G Graus", ang); //Exibe a informação no LCD
   delay_ms (500); //Espera 5 segundos para fazer nova leitura

   output_high(led);
   delay_ms(500);
   output_low(led);
   delay_ms(500);

   f_Angulo = ang;

   // Inicialização de escrita na porta serial
   printf ("\n\n\r :: Controle de Posicionamento e Movimentacao com Motor de Passo ::\n\r");
   delay_ms(250);
   printf ("\n\n\r :: ...... [ Curso Tecnico em Mecatronica ] ...... ::\n\n\r");
   delay_ms(250);

   Inicializacao();         // função de inicialização do motor

   while ( True )
   {
      printf("Digite a posicao angular desejada: \n\r");
      gets(s_Posicao);
      f_Posicao = atof(s_Posicao);
      delay_ms(250);
      if ((f_Posicao <= 90 ) && (f_Posicao > -1))
      { 
         printf("Posicao atual:    %5.1f\n\r", f_Angulo);
         printf("Posicao desejada: %5.1f\n\r", f_Posicao);
         printf("Posicao VAL: %ld\n\r", val);

         if ( f_Posicao <= f_Angulo )
         {
            i1_Sentido = 0;
            printf("Sentido de giro: Horario\n\r");
         }
         else
         {
            i1_Sentido = 1;
            printf("Sentido de giro: Anti-Horario\n\r");
         }

         if ( i1_Sentido == 0 )            // sentido horário
         {
            while ( f_Angulo > ( f_Posicao + (f_Resolucao / i8_ModoDivisao)) )
            {
               // Escrita/Acionamento do Motor de Passo
               output_d(i8_Passo[k]);
               f_Angulo = f_Angulo - (f_Resolucao / i8_ModoDivisao);
               f_Linear = f_Angulo * 0.0025 / f_Resolucao;
               printf("Angulo atual: %5.1f :: Posicao linear: %5.4f\n\r", f_Angulo, f_Linear);      

               // Atualização do Passo
               if (k == i8_LimiteK)
               {
                  k = 0;
               }
               else
               {
                  k++;
                  val = read_adc (); //Le valor DO ADC
                  ang = p * val; //Resolve o angulo para o valor lido
                  printf (lcd_putc, "\fValor do Angulo \n  %G Graus",ang); //Exibe a informação no LCD
               }
               delay_ms(i16_Velocidade);
            }
         }

         if ( i1_Sentido == 1 )            // sentido anti-horário
         {
            while ( f_Angulo < f_Posicao )
            {
               // Escrita/Acionamento do Motor de Passo
               output_d(i8_Passo[k]);
               f_Angulo = f_Angulo + (f_Resolucao / i8_ModoDivisao);
               f_Linear = f_Angulo * 0.0025 / f_Resolucao;
               printf("Angulo atual: %5.1f :: Posicao linear: %5.4f\n\r", f_Angulo, f_Linear);      

               // Atualização do Passo
               if (k == 0)
               {
                  k = i8_LimiteK;
               }
               else
               {
                  k--;
                  val = read_adc (); //Le valor DO ADC
                  ang = p * val; //Resolve o angulo para o valor lido
                  printf (lcd_putc, "\fValor do Angulo \n  %G Graus",ang); //Exibe a informação no LCD
               }
               delay_ms(i16_Velocidade);
            }
         }
      }
      else
      {
         printf("POSICAO ANGULAR INVALIDA! DIGITE VALORES ENTRE 0 E 90 GRAUS: \n\r");
      }
   }
}
