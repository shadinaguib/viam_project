import asyncio
from collections import deque
import aiohttp
from viam.robot.client import RobotClient
from viam.components.camera import Camera
from viam.services.vision import VisionClient
import time 

WEBSERVER_IP = "192.168.1.159"

async def connect():
    opts = RobotClient.Options.with_api_key( 
        api_key='your_api_key',
        api_key_id='your_api_key_id'
    )
    return await RobotClient.at_address('smart-trash-main.0f960q0n3l.viam.cloud', opts)

async def configure_lights(class_name):
    """
    Turn on and off lights based on the class_name, asynchronously.
    """
    print(f"Turning on lights for {class_name}")
    params = {'type': class_name}
    url_on = f"http://{WEBSERVER_IP}/trash/command"
    url_off = f"http://{WEBSERVER_IP}/trash/off"

    async with aiohttp.ClientSession() as session:
        start = time.time()
        print(time.time())
        async with session.post(url_on, params=params) as response:
            print(await response.text())
        await asyncio.sleep(3)
        print("Turning off lights")
        print(time.time() - start)
        async with session.post(url_off) as response:
            print(await response.text())

async def main():
    machine = await connect()
    print('Resources:', machine.resource_names)

    my_camera = Camera.from_robot(machine, "my-camera")
    yolo_trash = VisionClient.from_robot(machine, "yolo_trash_detector")
    detection_history = deque(maxlen=3)

    while True:
        print("Capturing image...")
        image = await my_camera.get_image()
        detections = await yolo_trash.get_detections(image)

        # Process detections
        current_frame_detections = {}
        for d in detections:
            if d.confidence > 0.5:
                current_frame_detections[d.class_name] = current_frame_detections.get(d.class_name, 0) + 1
                print(f"Detected: {d.class_name}")
                break  # Stops after the first valid detection

        for class_name in current_frame_detections:
            if class_name != "paper" and class_name != "cardboard":
                detection_history.append(class_name)

        print(f"Detection History: {detection_history}")
        if len(detection_history) == detection_history.maxlen:
            most_common = max(set(detection_history), key=detection_history.count)
            if detection_history.count(most_common) == detection_history.maxlen:
                await configure_lights(most_common)
                detection_history.clear()
                print("Resetting detection history and entering cooldown...")
                await asyncio.sleep(3)  # Cooldown period

    await machine.close()

if __name__ == '__main__':
    asyncio.run(main())
