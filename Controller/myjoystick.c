/**
 * Referred by a code from Jason White
 * Customized by Ramtin Alikhani
 *
 * Description:
 * Reads joystick/gamepad events and displays them.
 *
 * Compile:
 * gcc joystick.c -o joystick
 *
 * Run:
 * ./joystick [/dev/input/jsX]
 *
 * See also:
 * https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */
#include <fcntl.h>
#include <stdio.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>


/**
 * Reads a joystick event from the joystick device.
 *
 * Returns 0 on success. Otherwise -1 is returned.
 */
int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * Returns the number of axes on the controller or 0 if an error occurs.
 */
size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/**
 * Returns the number of buttons on the controller or 0 if an error occurs.
 */
size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/**
 * Current state of an axis.
 */
struct axis_state {
    short x, y;
};


/**
 * Keeps track of the current axis state.
 *
 * NOTE: This function assumes that axes are numbered starting from 0, and that
 * the X axis is an even number, and the Y axis is an odd number. However, this
 * is usually a safe assumption.
 *
 * Returns the axis that the event indicated.
 */
size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

    return axis;
}

int main(int argc, char *argv[])
{
    const char *device;
    int js;
    struct js_event event;
    struct axis_state axes[3] = {0};
    size_t axis;

    int leftMotorSpeed = 0;
    int rightMotorSpeed = 0;
    
    int stepperMotor = 0;
    int servoMotor = 0;
    
    int fd;
    char * myfifo = "/tmp/myfifo";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

    /* write "Hi" to the FIFO */
    fd = open(myfifo, O_WRONLY);


    while (1){
        
        
        if (argc > 1)
            device = argv[1];
        else
            device = "/dev/input/js0";

        js = open(device, O_RDONLY);

        if (js == -1)
            perror("Could not open joystick");

        size_t axis_count = get_axis_count(js);
        printf("axis count: %ld\n", axis_count);	

        /* This loop will exit if the controller is unplugged. */
        while (read_event(js, &event) == 0)
        {
            switch (event.type)
            {
                case JS_EVENT_BUTTON:
                    if (event.number == 0) //Green Button A is pressed 
                    {
                        char output[100];
                        
                        sprintf(output, "BA%s00000000000000000$", event.value ? "1" : "0");
                        write(fd, output, 23);
                        
                    }
                    break;
                case JS_EVENT_AXIS:
                    axis = get_axis_state(&event, axes);
                    if (axis = 1)   //Right analog (DC motors)
                    
                        if ( axes[axis].x == 0 )    //Straight forward or reverse (No turning)
                        {
                            rightMotorSpeed = round( axes[axis].y * ( -1 / 327.67 ) );
                            leftMotorSpeed = rightMotorSpeed;
                        }
                        if ( axes[axis].y == 0 || axes[axis].x == 0 )    //No forward or reverse (Only turning --> motors are complete opposite)
                        {
                            if ( axes[axis].x < 0 )    //Turning left (left motor reverse, right motor forward)
                            {
                                leftMotorSpeed = round( axes[axis].x * ( 1 / 327.67 ) );
                                axes[axis].x = 2 * leftMotorSpeed - 100;
    
                            }
                            else if ( axes[axis].x > 0 )    //Turning right (right motor reverse, left motor forward)
                            {
                                rightMotorSpeed = round( axes[axis].x * ( 1 / 327.67 ) );
                                axes[axis].y = -2 * leftMotorSpeed - 100;

                            }
                        }
    

                    if (axis = 0)   //Left  analog (Camera)
                    
                        if ( (axes[axis].x >= -2) && (axes[axis].x <= 0) && axes[axis].y <= 32767 )    //Tilt down
                        {
                            stepperMotor = round( axes[axis].x * ( -1 / 327.67 ) );
                            servoMotor = round( axes[axis].y * ( -1 / 327.67 ) );
                        }
                        if (( axes[axis].x >= -2) && (axes[axis].x <= 0) && axes[axis].y >= -32767 )    //Tilt up
                        {
                            stepperMotor = round( axes[axis].x * ( -1 / 327.67 ) );
                            servoMotor = round( axes[axis].y * ( -1 / 327.67 ) );
                        }
                        if (( axes[axis].y >= -2) && (axes[axis].y <= 0) && axes[axis].x <= 32767  )    //Pan Right
                        {
                            stepperMotor = round( axes[axis].x * ( -1 / 327.67 ) );
                            servoMotor = round( axes[axis].y * ( -1 / 327.67 ) );
                        }
                        if (( axes[axis].y >= -2) && (axes[axis].y <= 0) && axes[axis].x >= -32767  )    //Pan Left
                        {
                            stepperMotor = round( axes[axis].x * ( -1 / 327.67 ) );
                            servoMotor = round( axes[axis].y * ( -1 / 327.67 ) );
                        }
                        
                        char output[100];
                        
                        sprintf(output, "AL%+04d%+04d$AR%+04d%+04d$", stepperMotor, servoMotor, leftMotorSpeed, rightMotorSpeed);
                        write(fd, output, 23);
    //                      return (stepperMotor, servoMotor, leftMotorSpeed, rightMotorSpeed);

                    break;
                default:
                    
                    break;
            }
        }
    }
        /* remove the FIFO */
    unlink(myfifo);
        close(js);
    return 0;
}
    
int send()
{
      
    return 0;
}



