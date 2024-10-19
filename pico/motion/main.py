import network
from machine import Pin, WDT, disable_irq, enable_irq
import time
from wlan_config import WLAN_SSID, WLAN_PASSWORD
from mqtt import MQTTClient

mqtt_server = "192.168.1.104"  # Replace with your MQTT broker's address
client_id = "office_motion"  # Replace with your client ID
topic = "office/lights"  # Replace with the MQTT topic you want to publish to
inactive_time = 300  # Time in seconds before sending inactivity event

# keep interrupt prev state
state = None

# STATES
# 0 = DISABLED
# 1 = LISTENING
# 2 = ON (NO ACTION)
CURRENT_STATE = 1

# Set sensor pin for enabling and disabling sensor
MOTION_SENSOR_VALUE_PIN = 4
OVERRIDE_BUTTON_PIN = 1
motion_sensor_in = Pin(MOTION_SENSOR_VALUE_PIN, Pin.IN)
override = Pin(OVERRIDE_BUTTON_PIN, Pin.IN, Pin.PULL_UP)

# Set the LED high (turn it on)
led = Pin("LED", Pin.OUT)
led.off()
led.on()

# Set watchdog timer
wdt = WDT(timeout=8000)


# Callback function to disable sesor reading
# def on_message(client, userdata, message):
#     global state
#     state = message.payload.decode()
#     print(f"Setting state to {state}")

#     # Convert mqtt message to int
#     try:
#         enable = int(state)
#     except ValueError as verr:
#         return

#     print(enable)
#     # Ignore if not 0 or 1
#     if enable > 1 or enable < 0:
#         return

#     if enable == 1:
#         state = disable_irq()
#     elif enable == 0 and state != None:
#         enable_irq(state)


# Create a Wi-Fi interface
wlan = network.WLAN(network.STA_IF)

# Activate the Wi-Fi interface
wlan.active(True)

# Connect to the Wi-Fi network
wlan.connect(WLAN_SSID, WLAN_PASSWORD)

# Wait for a successful connection
while not wlan.isconnected():
    pass

# Print network configuration
print("Connected to:", WLAN_SSID)
print("Network Configuration:", wlan.ifconfig())

client = MQTTClient(client_id, mqtt_server)
client.connect()
print("Connected to MQTT broker")

active_time = 0


# Trigger for motion sensor interrupt
def INT_trigger_publish(pin):
    global CURRENT_STATE
    global active_time

    # Dont activate lights if state is disabled
    if CURRENT_STATE != 0:
        active_time = 0
        connect_and_publish("FOCUS")
        CURRENT_STATE = 2


last_debounce_time = 0
debounce_delay = 50  # ms


def debounce(pin):
    global last_debounce_time
    current_time = time.ticks_ms()

    if current_time - last_debounce_time > debounce_delay:
        last_debounce_time = current_time
        return True
    return False


# Trigger for override button interrupt
def INT_override_button(pin):
    global CURRENT_STATE

    # Account for sensitivity of button
    if debounce(pin):
        if CURRENT_STATE == 0:
            # Return to listening state
            CURRENT_STATE = 1
            print(f"Reactivating, State: {CURRENT_STATE}")
            led.on()
        else:
            CURRENT_STATE = 0
            led.off()
            print(f"State set to: {CURRENT_STATE}")
            connect_and_publish("OFF")


def connect_and_publish(state):
    global client
    client.publish(topic, state)
    print("PUBLISHED:", topic, state)


# Define interrupts
motion_sensor_in.irq(trigger=Pin.IRQ_RISING, handler=INT_trigger_publish)
override.irq(trigger=Pin.IRQ_FALLING, handler=INT_override_button)

# Subscribe to ENABLE topic to toggle sensor function
# client.cb = on_message
# client.subscribe("ENABLE")

while True:
    time.sleep_ms(1)
    if CURRENT_STATE == 2:
        active_time += 5
        print(active_time)

        # Trigger deactivate event if inactive_time is exceeded
        if active_time > inactive_time:
            print("Deactivating")
            connect_and_publish("OFF")
            CURRENT_STATE = 1

        time.sleep(5)  # Optional: Add a delay to prevent rapid reading

    wdt.feed()
