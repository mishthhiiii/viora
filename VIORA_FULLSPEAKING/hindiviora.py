import sounddevice as sd
import numpy as np
import speech_recognition as sr

def listen_hindi_once(timeout=5, phrase_time_limit=6):
    r = sr.Recognizer()

    # Record audio using sounddevice (NO PYAUDIO REQUIRED)
    print("🎧 सुन रहा हूँ...")

    try:
        audio_data = sd.rec(
            int(timeout * 16000),
            samplerate=16000,
            channels=1,
            dtype='int16'
        )
        sd.wait()

        audio_np = np.squeeze(audio_data)
        audio_bytes = audio_np.tobytes()
        audio = sr.AudioData(audio_bytes, 16000, 2)
    except Exception as e:
        print("❌ sounddevice error:", e)
        return ""

    try:
        text = r.recognize_google(audio, language="hi-IN").lower()
        print("🗣️ सुना:", text)
        return text
    except:
        print("❌ समझ नहीं आया")
        return ""
4