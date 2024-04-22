# AI-assisted smart trash with VIAM.

https://github.com/shadinaguib/viam_project/assets/26313021/9b3280e2-b059-4f65-91dc-b07d1d67cb5f

## Introduction 

For a tutorial on how to build this project, [click here](https://www.instructables.com/Build-an-AI-Assisted-Smart-Trash-System-With-VIAM/) for the instructables link.

## Step 1 : Setting up the object detection model
### Configure your physical camera
Configure your webcam so that your machine can get the video stream from the camera:

1. On the Viam app, navigate to your machine’s page. Check that the part status dropdown in the upper left of the page, next to your machine’s name, reads “Live”; this indicates that your machine is turned on and that its instance of viam-server is in contact with the Viam app.
2. Click the + (Create) button next to your main part in the left-hand menu and select Component. Start typing “webcam” and select camera / webcam. Give your camera a name. This tutorial uses the name my_webcam in all example code. Click Create.
3. Click the video path dropdown and select the webcam you’d like to use for this project from the list of suggestions.
4. Click Save in the top right corner of the screen to save your changes.

### Test your physical camera
To test your camera, go to the CONTROL tab and click to expand your camera’s panel.

Toggle View my_webcam to the “on” position. The video feed should display. If it doesn’t, double-check that your config is saved correctly, and check the LOGS tab for errors.

### Configure the vision service
Now that you know the camera is properly connected to your machine, it is time to add computer vision by configuring the vision service on your machine. Viam’s built-in mlmodel vision service works with Tensor Flow Lite models, but since this tutorial uses a YOLOv8 model, we will use a module from the modular resource registry that augments Viam with YOLOv8 integration. The YOLOv5 module enables you to use any YOLOv5 model with your Viam machines.

1. Navigate to your machine’s CONFIGURE tab.
2. Click the + (Create) button next to your main part in the left-hand menu and select Service. Start typing yolo and select vision / yolov8 from the registry options. Click Add module.
3. Give your vision service a name, for example yolo, and click Create.
4. In the attributes field of your new vision service, paste the following JSON:
```
{
  "model_location": "keremberke/yolov5m-garbage"
}
```
### Configure the objectfilter module
The physical camera is working and the vision service is set up. Now you will pull them together with the objectfilter module. This module takes a vision service (in this case, your hard hat detector) and applies it to your webcam feed. It outputs a stream with bounding boxes around the hard hats (and people without hard hats) in your camera’s view so that you can see the detector working. This module also filters the output so that later, when you configure data management, you can save only the images that contain people without hard hats rather than all images the camera captures.

1. Navigate to your machine’s CONFIGURE tab.
2. Click the + (Create) button next to your main part in the left-hand menu and select Component. Start typing objectfilter and select camera / objectfilter from the results. Click Add module.
3. Name your filtering camera something like objectfilter-cam and click Create.
4. Paste the following into the attributes field:
```
{
  "display_boxes": true,
  "camera": "my-camera",
  "filter_data": true,
  "vision_services": [
    "yolo_trash_detector"
  ],
  "labels": [
    "biodegradable",
    "glass",
    "metal",
    "plastic"
  ],
  "confidence": 0.5
}
```
Your objectfilter camera configuration should now resemble the following:
![image](https://github.com/shadinaguib/viam_project/assets/26313021/8de8d15a-aaf3-4b15-bd2d-f50d56b38e57)


### Test the detector 
Now that the detector is configured, it’s time to test it!

Navigate to the CONTROL tab.
Click the objectfilter_cam panel to open your detector camera controls.
Toggle View objectfilter_cam to the “on” position. This displays a live feed from your webcam with detection bounding boxes overlaid on it.

## Step 2: ESP32 WebServer
Connect the LED strip to your ESP32, and upload esp32_webserver.jino code to initiate a webserver.
In order to test it out, try going opening a browser window and going to :
```
http://<your_ip>/trash/off
```
If the LEDs turn red, then your WebServer is working!

## Step 3 : VIAM code
The only thing left to do is run a VIAM script that will send a custom message to the webserver depending on the detection made. In our case, we have to detect the item in three consecutive frames to turn the LEDs on.

Run the viam-server : 
```
viam-server -config ~/Downloads/viam-smart-trash-main.json
```

Run the running_script.py file : 
```
python running_script.py
```
Depending on what you show your webcam, the right trash can should turn green!
