import network
import machine
import time
from mqtt import MQTTClient

mqtt_server = "192.168.0.120"   # Replace with your MQTT broker's address
client_id = "office_motion"     # Replace with your client ID
topic = "office/lights"         # Replace with the MQTT topic you want to publish to
inactive_time = 300             # Time in seconds before sending inactivity event

CURRENT_STATE = 1

# Set sensor pin for enabling and disabling sensor
MOTION_SENSOR_SUPPLY_PIN = 12
MOTION_SENSOR_VALUE_PIN = 4
motion_sensor_out = machine.Pin(MOTION_SENSOR_SUPPLY_PIN, machine.Pin.OUT)
motion_sensor_in = machine.Pin(MOTION_SENSOR_VALUE_PIN, machine.Pin.IN)

# Callback function to disable sesor reading
def on_message(client, userdata, message):
    global CURRENT_STATE
    state = message.payload.decode()
    print(f"Setting state to {state}")

    # Convert mqtt message to int
    try:
        enable = int(state)
    except ValueError as verr:
        return

    # Ignore if not 0 or 1
    if enable > 1 or enable < 0:
        return
    
    # Enable/disable motion sensor by setting high/low
    CURRENT_STATE = enable
    motion_sensor_out.value(enable)

# Create a Wi-Fi interface
wlan = network.WLAN(network.STA_IF)


# Activate the Wi-Fi interface
wlan.active(True)

# Specify your Wi-Fi SSID and password
ssid = ""
password = ""

# Connect to the Wi-Fi network
wlan.connect(ssid, password)

# Wait for a successful connection
while not wlan.isconnected():
    pass

# Print network configuration
print("Connected to:", ssid)
print("Network Configuration:", wlan.ifconfig())

client = MQTTClient(client_id, mqtt_server)
client.connect()
print("Connected to MQTT broker")

def connect_and_publish(state):
    global client
    client.publish(topic, state)
    print("PUBLISHED:", topic, state)

# Subscribe to ENABLE topic to toggle sensor function
client.cb = on_message
client.subscribe("ENABLE")

active_time = 0
while True:
    # TODO: Perform interrupt when motion sensor is enabled/disabled

    if CURRENT_STATE != 0:
        motion_detected = motion_sensor_in.value()  # Read the GPIO pin value
        
        if motion_detected and CURRENT_STATE != 2:
            connect_and_publish("FOCUS")
            CURRENT_STATE = 2

            active_time = 0
        else:
            print("No motion.")

            if CURRENT_STATE == 2:
                active_time += 5

                # Trigger deactivate event if inactive_time is exceeded
                if active_time > inactive_time:
                    connect_and_publish("OFF")
                    CURRENT_STATE = 1
        
        time.sleep(5)  # Optional: Add a delay to prevent rapid reading
