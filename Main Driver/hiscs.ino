#include <SPI.h>

int mVperAmp = 100;

double Voltage = 0;
double VRMS = 0;
double Current = 0;
double av_current = 4 ;
double new_maxcurrent = 4 ;
int load[4] = {7,6,5,4} ;
int priority[4] = {0,1,2,3};
int state[4] = {1,1,1,1};
const int sensor[] = {A0,A1,A2,A5};

char data = 0;
byte oled_data;
unsigned long start_time[4] = {0, 0, 0, 0};
unsigned long schedule_time[4] = {0, 0, 0, 0};
int schedule_state[4] = {1, 1, 1, 1};
int schedule_enabled[4] = {0, 0, 0, 0};

int table[24] = {1234,1243,1324,1342,1423,1432,2134,2143,2314,2341,2413,2431,3124,3142,3214,3241,3412,3421,4123,4132,4213,4231,4312,4321};

void setup(){ 
 Serial.begin(9600);
 

 digitalWrite(SS, HIGH); // disable Slave Select
 SPI.begin ();
 SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8

 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 pinMode(6, OUTPUT);
 pinMode(7, OUTPUT);
}

void send_data(byte Mastersend)
{
   byte Masterget ;
   digitalWrite(SS, LOW); // enable Slave Select
   Masterget = SPI.transfer(Mastersend);
   digitalWrite(SS, HIGH); // disable Slave Select
   delay(1000);
}

byte findindex()
{
  int num, index = 0;
  num = ((priority[0]+1) * 1000) + ((priority[1]+1) * 100) + ((priority[2]+1) * 10) + priority[3]+1 ;
  for(int count = 0; count < 24; count++)
  {
    if(table[count] == num)
    {
      index = count ;
    }
  }
  return byte(index) ;
}



void set_schedule(int con, int con_state, unsigned long con_time)
{
  start_time[con] = millis() / 1000;
  schedule_state[con] = con_state;
  schedule_time[con] = con_time;
  schedule_enabled[con] = 1;
}

void cancelAllSchedules()
{
  for (int m = 0; m < 4; m++) {
    schedule_enabled[m] = 0;
  }
  sendData();
}

void switchLoad(int loadIndex, bool s)
{
  digitalWrite(load[loadIndex], s ? HIGH : LOW);
  state[loadIndex] = s ? 1 : 0;
}

void reInit()
{
  for(int w = 0; w < 4; w++)
  {
    state[w] = 1;
  }
}

void loop()
{
  av_current = new_maxcurrent ;
  
  // Priority Evaluation
  int j;
  for(j = 0; j < 4; j++)
  {
    if(state[priority[j]])
    {
      switchLoad(priority[j], true);
      delay(50);
      Current = getCurrent(priority[j]);

      if(Current <= av_current)
      {
        av_current -= Current;
      }
      else
      {
        switchLoad(priority[j], false);
        sendData();
      }
    }
  }
    
  // Schedule Execution
  for(int c = 0 ; c < 4 ; c++)
  {
    if (!schedule_enabled[c]) continue;
    if(millis() < 2000) continue;
    
    if((schedule_time[c] + start_time[c]) <= millis() / 1000 ) // PHONE TUL HOO!!!!!!!!!!!!!!!!!!!
    {
        switchLoad(c, schedule_state[c] == 1);
        schedule_enabled[c] = 0;
        sendData();
        //Serial.println("gov haa");
        // Serial.println(c);
    }
  }
  // -- Schedule Execution

   
 // Serial.println(send_string);
  if(Serial.available() > 0)  
  {
    String value = Serial.readString();
    char command = value.charAt(0);
    switch (command) {
      case 'P':
        for (int r = 1; r < 5; r++) {
          priority[r-1] = int(value.charAt(r) - '0');
        }
        
        oled_data = findindex();
        send_data(oled_data);
        reInit();
        break;
      case 'X':
        sendData();
        break;
      case 'Z':
        cancelAllSchedules();
        break;
      case 'T':
        {
          char load_sel = value.charAt(1);
          int load_st = int(value.charAt(2) - '0');
          int q = int(value.charAt(3) - '0');
          int w = int(value.charAt(4) - '0');
          int e = int(value.charAt(5) - '0');
          int r = int(value.charAt(6) - '0');
          int t = int(value.charAt(7) - '0');
          unsigned long seconds = (q * 10000) + (w * 1000) + (e * 100) + (r * 10) + t;
          set_schedule(int(load_sel - 'A'), load_st, seconds);
        }
        break;
      
      case 'S':
        {
          char load_select = value.charAt(1);
          char load_state = value.charAt(2);
          switch (load_select){
            case 'A':
            case 'B':
            case 'C':
            case 'D':
              switchLoad(int(load_select - 'A'), load_state == '1');
              break;
            case 'R':
               reInit();
               break;
            case 'I':
               send_data(50 + load_state - '0');
               break;
            case 'M':
              int tens = int(load_state - '0');
              int ones = int(value.charAt(3) - '0');
              int dec1 = int(value.charAt(4) - '0');
              int dec2 = int(value.charAt(5) - '0');
              new_maxcurrent = tens * 10  + ones  + dec1 * 0.1 + dec2 * 0.01;
              reInit();
              break;
          }

          sendData();
        }
        break;
      
    }


    
  }


  
}


void sendData()
{
  String scheduleStates = "";
  for (int z = 0; z < 4; z++)
  {
    scheduleStates += String(schedule_enabled[z]);
  }
  
  String send_string = String("") + 
                       "A" + state[0] + getCurrent(0) + ";" + 
                       "B" + state[1] + getCurrent(1) + ";" + 
                       "C" + state[2] + getCurrent(2) + ";" + 
                       "D" + state[3] + getCurrent(3) + ";" + 
                       scheduleStates;
  
  Serial.println(send_string); 
}

float getVPP(int sensor)
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 100) 
   {
       readValue = analogRead(sensor);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = (((maxValue - minValue) * 5.0) / 1024.0) ;
   
   return result;
}

float getCurrent(int k)
{
  Voltage = getVPP(sensor[k]);
  VRMS = (Voltage/2.0) * 0.707; 
  return (((VRMS * 1000) / mVperAmp));
}
