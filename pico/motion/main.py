import network
import machine
import time
from mqtt import MQTTClient

mqtt_server = "192.168.0.120"  # Replace with your MQTT broker's address
client_id = "office_motion"  # Replace with your client ID
topic = "office/lights"  # Replace with the MQTT topic you want to publish to

# Set sensor pin for enabling and disabling sensor
MOTION_SENSOR_SUPPLY_PIN = 12
MOTION_SENSOR_VALUE_PIN = 4
motion_sensor_out = machine.Pin(MOTION_SENSOR_SUPPLY_PIN, machine.Pin.OUT)
motion_sensor_in = machine.Pin(MOTION_SENSOR_VALUE_PIN, machine.Pin.IN)

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

def connect_and_publish():
    global client
    client.publish(topic, "FOCUS")
    print("PUBLISHED:", topic)

# Subscribe to ENABLE topic to toggle sensor function
client.subscribe("ENABLE")
def on_message(client, userdata, message):
    state = message.payload.decode()
    print(f"Setting state to {state}")

    # Set sensor pin high/low
    try:
        enable = int(state)
    except ValueError as verr:
        return

    # Ignore if not 0 or 1
    if enable > 1 or enable < 0:
        return
    
    # Enable/disable motion sensor
    motion_sensor_out.value(enable)


last_state_on = False
while True:
    motion_detected = motion_sensor_in.value()  # Read the GPIO pin value
    
    if motion_detected and not last_state_on:
        connect_and_publish()
        last_state_on = True
    else:
        print("No motion.")
        last_state_on = False
    
    time.sleep(5)  # Optional: Add a delay to prevent rapid reading
