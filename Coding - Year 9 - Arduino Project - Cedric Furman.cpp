
/*

April - June, 2021
Cedric Furman
This is my project 3 Arduino device: Interactive Gallery Lighting (IGL)
IGL aims to improve lighting systems at galleries: 

IGL adjusts based on natural light and visitors' distances to artwork; minimizing light exposure and improving lighting quality
IGL also uses lights to warn those at galleries when they are too close to art, and to alert of paint-harming heat

*/

/////////////////////////////// Setup ///////////////////////////////

// The following variables store the pin numnbers of each component:

int whiteLedPin = 6; // The main LED

int yellowLedPin = 8;

int redLedPin = 9;

int ultrasonicSensorTriggerPin = 10;

int ultrasonicSensorEchoPin = 11;

int pushbuttonPin = 12;

// The photodiode and temperature sensor use analog pins to collect numerical data

int temperatureSensorPin = A0;

int photodiodePin = A1;


// The components' pins are declared
void setup(void) {
  
    Serial.begin(9600);
  
    // LEDs have output pins because they only follow commands
  
  	pinMode(whiteLedPin, OUTPUT); 
  
    pinMode(yellowLedPin, OUTPUT); 
  
    pinMode(redLedPin, OUTPUT); 
  
    pinMode(ultrasonicSensorTriggerPin, OUTPUT); // The ultrasonic sensor trigger has an output pin because it only searches
  
    // The remaining components all have input pins since they return information
  
  	pinMode(ultrasonicSensorEchoPin, INPUT); 
  
    pinMode(pushbuttonPin, INPUT); 
  
    pinMode(photodiodePin, INPUT); 
  
    pinMode(temperatureSensorPin, INPUT); 

}
///////////////////////////// Functions /////////////////////////////

int pushbuttonStatus; // Stores whether the power button is pressed or not

bool powerBoolean = true; // A boolean used to return whether the Arduino is on or off


// This function checks if IGL is on or off using the pushbutton status
bool checkPower() {
  
    pushbuttonStatus = digitalRead(pushbuttonPin); // The status of the pushbutton is either on ("HIGH") or off ("LOW"), and is read using the "digitalRead" function
  
    if (pushbuttonStatus == HIGH) { // If the pushbutton is being pressed, the Arduino is on
      
        powerBoolean = true;
    
    }
  
    else { // If the pushbutton is not being pressed, the Arduino is off
      
        powerBoolean = false;
    
    }
  
    return powerBoolean;
  
}


bool start = false; // Stores whether the Arduino should start; based on whether IGL is on or off

// This function is used to indicate when IGL has been turned on or off
void changePowerState () { 
  
    // The red LED is briefly illuminated to signal that IGL has experienced a change in power state
  
    digitalWrite(redLedPin, LOW);
    
    delay(1000);
  
    digitalWrite(redLedPin, HIGH);
  
    // IGL's new power state is printed in the Serial Monitor
  
    start;
  
    if (start == true) {
    
        Serial.println("\nIGL has turned on");
    
    }
  
  else {
    
        Serial.println("\nIGL has turned off");
    
    }
  
    // The red LED is turned off in order to be used for alerts
  
    delay(1000);
  
    digitalWrite(redLedPin, LOW);
  
    delay(500);
  
}
  
// Ultrasonic distance sensor functions
// sources: https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6 and https://www.tinkercad.com/things/0WCWaVS4Awf-class-copy-of-4-pin-ultrasonic-do-not-edit/editel?sharecode=t8FnkqNPUQEVaIIuNzdttQjqcSN91_0hy91YaYskdx0

double brightness = 25; // Stores the brightness level, at which the main LED is displayed

double ultrasonicTriggerTime; // Holds the time it takes for the distance sensor's ultrasound wave to return after bouncing off a person

double visitorDistance; // Stores the newly detected distance of the closest person to a painting

double currentVisitorDistance; // Stores the current distance of the closest person to artwork (used to prevent infinite Serial printing)

// Uses the ultrasonic distance sensor to determine visitor distance, also changes the main LED's brightness accordingly
void checkVisitorDistance () {
  
    // An ultrasound wave is sent out

  	digitalWrite(ultrasonicSensorTriggerPin, LOW);
  
  	delay(1);
  
  	digitalWrite(ultrasonicSensorTriggerPin, HIGH);
  
  	delay(1);
  
  	digitalWrite(ultrasonicSensorTriggerPin, LOW);
  
    // The time it took for the ultrasound wave to return is recorded as "ultrasonicTriggerTime"
  
    ultrasonicTriggerTime = pulseIn(ultrasonicSensorEchoPin, HIGH);
  
    visitorDistance = (ultrasonicTriggerTime/2)/29.1; // Converts ultrasonicTriggerTime into centimeters
  
    // A conditional statement is used to print the visitor distance only when it changes
  
    if (visitorDistance <= currentVisitorDistance - 5 || visitorDistance >= currentVisitorDistance + 5) { // If the sensor detects a different distance to the current one, it's printed
    
      	currentVisitorDistance = visitorDistance; // Sets the newly detected different distance as the current distance
  
      	// The nearest visitor's distance is printed
  
      	Serial.println();
  
      	Serial.print("Visitor distance: ");
  
      	Serial.print(currentVisitorDistance);
      
      	Serial.println(" cm");
            
    }
  
}

float naturalLightDecimal = 1; // Stores the decimal of natural light, which affects the brightness of the main LED (more natural light means aftifical brightness can be reduced)

// Alters the main LED's brightness based on natural light
void adjustBrightnessToDistance () {
  
    brightness = 25 * naturalLightDecimal; // Resets the brightness variable (so it doesn't get exponentially larger)
  
    // If brightness is at its highest—when the visitor is closest to artwork; brightness is simply 25, the minimum, times the nautral light decimal
    
    if (visitorDistance < 100) {
      
        brightness = 255 * naturalLightDecimal;
              
    }
  
    // If the visitor distance also affects brightness, a larger calculation is used
  
    else if (visitorDistance >= 100) {
      
        // Brightness is 25 plus the visitor distance rate [(336 / visitor distance) x 2.95: a rate that incriments up to maximum brightness based on visitor distance, when more than 100 cm], and multiplied by the natural lgiht decimal
            
        brightness = (25 * (2.95 * (336.0 / visitorDistance))) * naturalLightDecimal; 
                          
    }
  
}

// Main LED function

// Illuminates the main LED 
void illuminate () {
  
    // An integer of brightness is taken and used to change the brightness display of the main LED, using Pulse Width Modulation (PWM) with "analogWrite"
  
    long currentBrightness = brightness; 
    
    analogWrite(whiteLedPin, currentBrightness); 
  
}


// Photodiode function

int currentNaturalLight = 0; // Stores the current natural amount 

// Determines the natural light amount 
void adjustBrightnessToNaturalLight () {
  
    int naturalLight = analogRead(photodiodePin); // Natural light is the analog input of the photodiode 
  
    // Similar to printing the visitor distance, only changes in natural light are printed
  
    if (naturalLight != currentNaturalLight) { // If a new level of natural light is inputted by the photodiode, it's printed
      
        currentNaturalLight = naturalLight;
  
      	Serial.print("\nPhotodiode input: ");
  
      	Serial.println(currentNaturalLight);
      
        Serial.println();
  
    } 
  
    // Converts the natural light amount into a decimal (used above to adjust the main LED's brightness)
  
    naturalLightDecimal = ((100 - naturalLight) / 100.0);
      
    brightness *= naturalLightDecimal;
  
}

// Temperature function
// Source: https://learn.adafruit.com/tmp36-temperature-sensor/using-a-temp-sensor?view=all

float currentTemperature = 0.0; // Stores the temperature in degrees celsius (C)

// Converts the temperature sensor input into the current temperature
void convertTemperature () {
  
    // A fourmula is used to convert temperature sensor input into celsuis (from the source above)
  
    float temperature = ((((analogRead(temperatureSensorPin) * 5.0) / 1024.0) - 0.5) * 100.0); 
  
    // As with visitor distance and natural light, only changes in temperature are printed
  
    if (temperature != currentTemperature) { 
            
        currentTemperature = temperature;
  
      	Serial.print("\nTemperature: ");
  
      	Serial.print(currentTemperature);
      
        Serial.println(" C");
      
        Serial.println();
  
    } 
  
}

// Alert functions

// A function used to alert of worrying circumstances (visitors being too close to art and paint-damaging temperatures)
void alert () {
  
    // If the more extremem circumstances of either alert occur, the red LED is blinked

    if (visitorDistance < 50 || currentTemperature >= 40) {
      
        // Alert LEDs are reset
      
        digitalWrite(yellowLedPin, LOW);
      
        digitalWrite(redLedPin, LOW);
      
        // A delay is used to blink the red LED
          
        delay(400);
          
        digitalWrite(redLedPin, HIGH);
      
        return; // The function returns to avoid the next conditional statement
    
    }
  
    // In the case of less worrying circumstances, the yellow LED is used to send a warning
  
    else if (visitorDistance < 100 || currentTemperature >= 30) {
      
        // The red LED is turned off, and the yellow one is illuminated
      
        digitalWrite(redLedPin, LOW);
      
        digitalWrite(yellowLedPin, HIGH);
          
    }
  
    // In regular circumstances, the alert LEDs are turned off
  
    else {
      
        digitalWrite(yellowLedPin, LOW);
      
        digitalWrite(redLedPin, LOW);
          
    }
  
}

///////////////////////////// Main Loop //////////////////////////////

void loop() {
  
    // IGL waits for the power button to be pressed before starting

    while (start == false) {
      
        // Once IGL is turned on, "start" is set to true so the loop continues uninterrupted 
          
        if (checkPower() == true) {
          
            start = true;
           
          	changePowerState(); // Blinks the red LED once to signify when IGL is turned on
        
        }
      
    }
    
    if (start == true) {
       
        // Main loop
      
        checkVisitorDistance();
      
        adjustBrightnessToDistance();
      
        illuminate();
      
        adjustBrightnessToNaturalLight();
      
        convertTemperature();
      
        alert();
      
        // Checks if IGL is turned off
      
        if (checkPower() == true) {
          
            // if IGL is turned off, "start" is reset to false so IGL waits for to be started at the beginning of the loop
    
        	start = false;
                    
            changePowerState(); // Just as when turning on, IGL blinks a red LED once when turning off
    
        }
    
    }

}
