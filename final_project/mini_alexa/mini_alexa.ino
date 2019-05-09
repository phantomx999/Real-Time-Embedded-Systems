/*
 
OPTIONAL:
To reduce the library compiled size and limit its memory usage, you
can specify which shields you want to include in your sketch by
defining CUSTOM_SETTINGS and the shields respective INCLUDE_ define. 

*/

#define CUSTOM_SETTINGS
#define INCLUDE_INTERNET_SHIELD
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD
#define INCLUDE_MUSIC_PLAYER_SHIELD
#define INCLUDE_CLOCK_SHIELD


/* Include 1Sheeld library. */
#include <OneSheeld.h>

/* create an initial value for the millis counter. */
unsigned long previousMillis = 0;
/* create an intial state for calling alexa. */
bool state = 0;

/* Call an api that will return the weather status in minneapolis,mn. */
HttpRequest request2("http://api.openweathermap.org/data/2.5/weather?q=Minneapolis,US&APPID=8bcc571d4ed792ed0264e3ef3e59335f");
HttpRequest request1("https://maps.googleapis.com/maps/api/distancematrix/json?origins=eden prairie, mn&destinations=minneapolis, mn&departure_time=now&key=AIzaSyDXRdLbus3yzuq21PQk0qCI67fekR27Wdg");

/* define the voice recognition on pin 13. */
int voice_on = 13;

/* define the variable that will hold the hours and miniuts . */
int hour, minute;

/* Create 2 charachter arrys to catch the string you want from the sentence. */
char h[4];
char m[4];
char d[4];
char f[4];

/* define variables to hold the hour and miniuts in string. */
String strh, strm, distancem, kelvinm;
/* define variable  to hold the day status am or pm. */
String noon;

/* Voice commands set by the user. */

const char mainCommand[] = "andy";

const char weatherCommand[] = "what is the temperature today";

const char trafficCommand[] = "what is the traffic now";

const char playCommand[] = "play music";

const char clockCommand[] = "what time is it now";

const char stopCommand[] = "stop";


void setup() {
  /* Start Communication. */
  OneSheeld.begin();
/* set the led strips and the lamp to output. */
  pinMode(voice_on,OUTPUT);

      

 /* Subscribe to success callback for the request. */
 request2.setOnSuccess(&onSuccess2);
 /* Subscribe to json value replies. */
 request2.getResponse().setOnJsonResponse(&onJsonReply2);

 /*get the traffic*/
 /* Subscribe to success callback for the request. */
 request1.setOnSuccess(&onSuccess1);
      
 /* Subscribe to json value replies. */
 request1.getResponse().setOnJsonResponse(&onJsonReply1);
 /* on a voice recogniced call the function "myfunction". */
 VoiceRecognition.setOnNewCommand(&myFunction);
}

void loop ()
{
  /* make a delay for 5 seconds using millis counter to keep the voice detection alive. */
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000) {

    previousMillis = currentMillis;

    /* get the current time in your phone. */
     Clock.queryDateAndTime();
     
     /* start voice recognition. */
    VoiceRecognition.start();
  }
/* save the smartphone hour and minutes in variables. */
  hour = Clock.getHours();
  minute = Clock.getMinutes();
  
/* detect the time status if it am or pm. */
  if (hour > 12)
  { hour -= 12;
    noon = "P M";
  }
  else {
    noon = "A M";
  }


/* save the hour and minutes as string. */
  strh = String(hour);
  strm = String(minute);

/* save the string in a charchter array. */
  strh.toCharArray(h, 4);
  strm.toCharArray(m, 4);
}

/* This function will be invoked each time a new command is given. */
void myFunction (char *commandSpoken)
{
/* check if any one call alexa or not. */
  if (!strcmp(mainCommand, VoiceRecognition.getLastCommand()))
  {
    andyon();  
    MusicPlayer.setVolume(5);
    /* if yes make the state =1 . */
    state = 1;
  }
  /* check if you asked to play music after calling alexa. */
  if (!strcmp(playCommand, VoiceRecognition.getLastCommand()) && state == 1)
  {

    andyoff();
    MusicPlayer.play();
    delay(10000);
    MusicPlayer.stop();
    state = 0;
    }
/* check if you asked to get the weather status after calling alexa. */
   else if(!strcmp(weatherCommand,VoiceRecognition.getLastCommand())&& state == 1)
    {

    andyoff();
    Internet.performGet(request2);
    state = 0;
    }
/* check if you asked for the time after calling alexa. */
  if(!strcmp(clockCommand,VoiceRecognition.getLastCommand()) && state == 1)
     {

      andyoff();
       /* 1Sheeld responds using text-to-speech. */
       TextToSpeech.say("time in minneapolis is");
       delay(1300);
       TextToSpeech.say(h);
        delay(600);
        TextToSpeech.say(m);
         delay(800);
         TextToSpeech.say(noon);
         delay(500);
          state = 0;
     }
/* check if you asked to turn the lamp on after calling alexa. */
  if (!strcmp(trafficCommand, VoiceRecognition.getLastCommand()) && state == 1)
  {
    andyoff();
    
    Internet.performGet(request1);
    state = 0;
  }
}

void onSuccess2(HttpResponse & response2)
{
  /* Using the response to query the Json chain till the required value. */
  /* i.e. Get the value of 'main' in the first object of the array 'weather' in the response. */
  /* Providing that the response is in JSON format. */
  response2["main"]["temp"].query();  
}

void onJsonReply2(JsonKeyChain & hell,char * output)
{

    float kelvin = atof(output);
    int fah = (kelvin - 273)*9/5 + 32;

    kelvinm = String(fah);

    /* save the string in a charchter array. */
    kelvinm.toCharArray(f, 4);
  
    TextToSpeech.say("the temperature today in minneapolis is");
    delay(2000);

    TextToSpeech.say(f);
   
}

void onSuccess1(HttpResponse & response1)
{
  /* Using the response to query the Json chain till the required value. */
  /* i.e. Get the value of 'main' in the first object of the array 'weather' in the response. */
  /* Providing that the response is in JSON format. */
  response1["rows"][0]["elements"][0]["duration_in_traffic"]["value"].query();
  /*response1["destination_addresses"].query();*/
   
}

void onJsonReply1(JsonKeyChain & hell,char * output)
{
  /* Getting the value and transform it to integer to deal with. */
  int eta = atoi(output);
  int mins = eta/60;
  distancem = String(mins);

  /* save the string in a charchter array. */
  distancem.toCharArray(d, 4);
  
   TextToSpeech.say("it will take");
    delay(2000);
    /* 1Sheeld responds using text-to-speech shield. */
    TextToSpeech.say(d);
    delay(2000);
    /* 1Sheeld responds using text-to-speech shield. */
    TextToSpeech.say("to drive to minneaplis from eden prairie");
  
}

void andyon(){
  /* Turn on led13 to show that the voice recognition is on*/
  digitalWrite(voice_on,HIGH); 
}

 

void andyoff(){
  /* Turn off the led13 to show andy got the command!. */
  digitalWrite(voice_on,LOW);
}
