import RPi.GPIO as GPIO
import time
import sys

# Morse Code Dictionary
MORSE_CODE_DICT = {
    'A': '.-', 'B': '-...', 'C': '-.-.', 'D': '-..', 'E': '.', 
    'F': '..-.', 'G': '--.', 'H': '....', 'I': '..', 'J': '.---', 
    'K': '-.-', 'L': '.-..', 'M': '--', 'N': '-.', 'O': '---', 
    'P': '.--.', 'Q': '--.-', 'R': '.-.', 'S': '...', 'T': '-', 
    'U': '..-', 'V': '...-', 'W': '.--', 'X': '-..-', 'Y': '-.--', 
    'Z': '--..', '1': '.----', '2': '..---', '3': '...--', '4': '....-', 
    '5': '.....', '6': '-....', '7': '--...', '8': '---..', '9': '----.', 
    '0': '-----'
}

# Pin Setup
LED_PIN = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)

def morse_code_for_letter(letter):
    """Get morse code sequence for a given letter."""
    return MORSE_CODE_DICT.get(letter.upper(), '')

def blink_morse_code(morse_code):
    """Blink the LED based on the Morse code."""
    for symbol in morse_code:
        if symbol == '.':
            GPIO.output(LED_PIN, GPIO.HIGH)
            time.sleep(0.2)
            GPIO.output(LED_PIN, GPIO.LOW)
            time.sleep(0.2)
        elif symbol == '-':
            GPIO.output(LED_PIN, GPIO.HIGH)
            time.sleep(0.6)
            GPIO.output(LED_PIN, GPIO.LOW)
            time.sleep(0.2)
        else:  # Space between letters
            time.sleep(0.6)

def main(word, repeat_count):
    for _ in range(repeat_count):
        for letter in word:
            morse_code = morse_code_for_letter(letter)
            blink_morse_code(morse_code)
            # Space between letters
            time.sleep(0.6)
        # Longer pause between repetitions
        time.sleep(.1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python myscript.py [word] [repeat_count]")
    else:
        word = sys.argv[1]
        try:
            repeat_count = int(sys.argv[2])
            main(word, repeat_count)
        except ValueError:
            print("Please provide a valid integer for repeat count.")
        finally:
            GPIO.cleanup()

