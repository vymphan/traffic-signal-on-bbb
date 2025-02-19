#include <stdio.h>          // Input/Output
#include <fcntl.h>          // File Controls
#include <unistd.h>         // sleep functions
#include <stdint.h>         // Fixed-width int type
#include <stdlib.h>         // System calls
#include <string.h>         // String-handling library
#include <sys/utsname.h>    // System info 

// Debug mode: #define DEBUG 1

#define GPIO_PATH   "/sys/class/gpio/"
// First set of traffic signal
#define RED_1       49
#define YELLOW_1    14
#define GREEN_1     15 
// Second set of traffic signal 
#define RED_2       48 
#define YELLOW_2    31 
#define GREEN_2     30

#define GPIO_OUT    "out"
#define GPIO_HIGH   "1" 
#define GPIO_LOW    "0"

// typedef Signal_t stores a set of 3 LEDs for each traffic signal
typedef struct {
    uint8_t id;         // ID for each unique signal
    uint8_t red;        // RED led gpio
    uint8_t yellow;     // YELLOW led gpio
    uint8_t green;      // GREEN led gpio
} Signal_t;

// Prototypes
static void setDirection        (const int gpio, const char * pDirection);
static void setValue            (const int gpio, const char * pValue);
static void turnOnLights        (const Signal_t * pSignal); 
static void turnOffLights       (const Signal_t * pSignal);
static void turnRed             (const Signal_t * pSignal);
static void turnYellow          (const Signal_t * pSignal);
static void turnGreen           (const Signal_t * pSignal);

// Main function
int main(void) 
{
#ifdef DEBUG
    // uname takes a pointer as an argument
    // In return, uname modifies the struct file with 
    // system name, node name and machine architecture
    struct utsname sysInfo;
    (void) uname(&sysInfo);
    // Print to console
    (void) printf("System Name: %s\n", sysInfo.sysname);
    (void) printf("Node Name: %s\n", sysInfo.nodename);
    (void) printf("Machine: %s\n", sysInfo.machine);
    (void) printf("Student's Name: Vy Phan\n");
    (void) printf("-----------------------------------------\n\n");
#endif
    
    // Set the direction of GPIOs to output
    setDirection(RED_1,     GPIO_OUT);
    setDirection(YELLOW_1,  GPIO_OUT);
    setDirection(GREEN_1,   GPIO_OUT);
    setDirection(RED_2,     GPIO_OUT);
    setDirection(YELLOW_2,  GPIO_OUT);
    setDirection(GREEN_2,   GPIO_OUT);

    // Set up two traffic signals
    Signal_t signalOne = { 1, RED_1, YELLOW_1, GREEN_1 };
    Signal_t signalTwo = { 2, RED_2, YELLOW_2, GREEN_2 }; 

    // Reset all LEDs to OFF state
    turnOffLights(&signalOne);
    turnOffLights(&signalTwo);

    // Start infinity loop
    while (1) 
    {
        // Traffic signal 1: RED
        // Traffic signal 2: GREEN   
        turnRed      (&signalOne);
        turnGreen    (&signalTwo);      
        (void) sleep(120);
        
        // Traffic signal 2: YELLOW
        turnYellow   (&signalTwo);
        (void) sleep(5);
        
        // Traffic signal 2: RED 
        // Traffic signal 1: GREEN 
        turnRed      (&signalTwo);
        turnGreen    (&signalOne);
        (void) sleep(120);

        // Traffic signal 1: YELLOW
        turnYellow   (&signalOne);
        (void) sleep(5);
    }   

    return 0;
}

// setDirection() function takes an integer of the gpio number
// and a string array pointer to set the direction of that gpio
static void setDirection(const int gpio, const char * pDirection )
{
#ifdef DEBUG
    (void) printf("DEBUG: Set \"%s\" to gpio%d direction\n", pDirection, gpio);
#else
    // Format the gpio path to set the direction
    char buffer[50] = "";
    (void) snprintf(buffer, sizeof(buffer), "%sgpio%d/direction", GPIO_PATH, gpio); 
    
    uint32_t fd = open(buffer, O_RDWR);         // Open file
    (void) write(fd, pDirection, 3);            // Write "out" direction  
    (void) close(fd);                           // close file
#endif
}

// setValue() function takes an integer of the gpio number
// and a string array pointer to set the value of that gpio
static void setValue(const int gpio, const char * pValue) 
{
#ifdef DEBUG
    (void) printf("DEBUG: Set \"%s\" to gpio%d value\n", pValue, gpio);
#else
    // Format the gpio path to set the direction
    char buffer[50] = "";
    (void) snprintf(buffer, sizeof(buffer), "%sgpio%d/value", GPIO_PATH, gpio);
    
    uint32_t fd = open(buffer, O_WRONLY);       // Open file
    (void) write(fd, pValue, 1);                // Write value to file
    (void) close(fd);                           // Close file
#endif
}

// Reset all LEDs by setting the values of all lights to low
static void turnOffLights(const Signal_t * pSignal)
{
#ifdef DEBUG
    (void) printf("DEBUG: Traffic signal %d: All lights OFF\n", pSignal->id);
#else
    // Write GPIO_LOW value to the gpio of the traffic signal    
    setValue(pSignal->red,      GPIO_LOW);  // Red off
    setValue(pSignal->yellow,   GPIO_LOW);  // Yellow off
    setValue(pSignal->green,    GPIO_LOW);  // Green off
#endif
}

// Turn on all lights by setting values of all lights to high
static void turnOnLights(const Signal_t * pSignal)
{
#ifdef DEBUG
    (void) printf("DEBUG: Traffic signal %d: All lights ON\n", pSignal->id);
#else    
    // Write GPIO_HIGH value to the gpio of the traffic signal
    setValue(pSignal->red,      GPIO_HIGH);  // Red on
    setValue(pSignal->yellow,   GPIO_HIGH);  // Yellow on
    setValue(pSignal->green,    GPIO_HIGH);  // Green on
#endif
}

// turnRed() take a Signal_t pointer and set Red LED to high,
// while yellow and green leds are set to low.
static void turnRed(const Signal_t * pSignal)
{
#ifdef DEBUG
    (void) printf("DEBUG: Traffic signal %d:\n", pSignal->id);
    (void) printf("        RED:    on\n");
    (void) printf("        YELLOW: off\n");
    (void) printf("        GREEN:  off\n");    
#else
    // Write GPIO_LOW value to gpio to turn off other lights 
    // Write GPIO_HIGH value to gpio to turn on red light 
    setValue(pSignal->yellow,   GPIO_LOW);  // Yellow off
    setValue(pSignal->green,    GPIO_LOW);  // Green off
    setValue(pSignal->red,      GPIO_HIGH); // Red on
#endif 
}


// turnYellow() take a Signal_t pointer and set Yellow LED to high,
// while red and green LEDs are set to low
static void turnYellow(const Signal_t * pSignal)
{
#ifdef DEBUG
    (void) printf("DEBUG: Traffic signal %d:\n", pSignal->id);
    (void) printf("        RED:    off\n");
    (void) printf("        YELLOW: on\n");
    (void) printf("        GREEN:  off\n");    
#else
    // Write GPIO_LOW value to gpio to turn off other lights
    // Write GPIO_HIGH value to gpio to turn on yellow light
    setValue(pSignal->red,      GPIO_LOW);      // Red off
    setValue(pSignal->green,    GPIO_LOW);      // Green off
    setValue(pSignal->yellow,   GPIO_HIGH);     // Yellow on
#endif 
}

// turnGreen() takes a Signal_t pointer and set the Green LED to high
// while the yellow and red LEDs are set to low
static void turnGreen(const Signal_t * pSignal)
{
#ifdef DEBUG
    (void) printf("DEBUG: Traffic signal %d:\n", pSignal->id);
    (void) printf("        RED:    off\n");
    (void) printf("        YELLOW: off\n");
    (void) printf("        GREEN:  on\n");    
#else
    // Write GPIO_LOW value to gpio to turn off other lights
    // Write GPIO_HIGH value to gpio to turn on green light
    setValue(pSignal->red,      GPIO_LOW);   // Red off
    setValue(pSignal->yellow,   GPIO_LOW);   // Yellow off
    setValue(pSignal->green,    GPIO_HIGH);  // Green off
#endif
}

