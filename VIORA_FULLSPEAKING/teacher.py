import speech_recognition as sr
import pygame
import google.generativeai as genai
import pyttsx3
import requests
import re
import time
from gtts import gTTS 
import os 

# API keys
GEMINI_API_KEY = "AIzaSyAc7hc2VXHk_TbmGHJxVawXUQPWjyIOYPI"
# ---------- CONFIG ----------
genai.configure(api_key=GEMINI_API_KEY)
pygame.mixer.init()

# ---------- SOUND ----------
def play_sound(file_path):
    pygame.mixer.music.load(file_path)
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(5)

# ---------- CLEAN TEXT ----------
def clean_text(text):
    text = re.sub(r'[*_`#•~<>\\-]', '', text)
    text = re.sub(r'http\S+', '', text)
    text = re.sub(r'\s+', ' ', text).strip()
    return text

#SPEECH TO TEXT

def listen_with_google():
    recognizer = sr.Recognizer()

    # Wake word variations (because Hindi STT changes pronunciation)
    wake_words = ["viora"]

    with sr.Microphone() as source:
        print("🎧 Listening...")
        audio = recognizer.listen(source)

        # Try English first for better accuracy of "Viora"
        try:
            text = recognizer.recognize_google(audio, language="en-IN").lower()
            print("🗣️ (EN detected):", text)
            return text
        except:
            pass

        # Try Hindi fallback
        try:
            text = recognizer.recognize_google(audio, language="hi-IN").lower()
            print("🗣️ (HI detected):", text)
            return text
        except:
            print("❌ Sorry, didn't catch that.")
            return ""


def text_to_speech(text, lang="auto", rate=150, volume=1.0):

    # Detect if text is Hindi (contains Devanagari letters)
    hindi_detected = any('\u0900' <= ch <= '\u097F' for ch in text)

    # If Hindi detected → use Google TTS
    if hindi_detected:
        print("🎤 Speaking (Hindi-natural):", text)
        tts = gTTS(text=text, lang='hi')
        filename = "temp_hindi.mp3"
        tts.save(filename)

        pygame.mixer.music.load(filename)
        pygame.mixer.music.play()
        while pygame.mixer.music.get_busy():
            pygame.time.Clock().tick(5)

        os.remove(filename)
        return

    # Otherwise → English → use pyttsx3
    engine = pyttsx3.init()
    voices = engine.getProperty("voices")
    engine.setProperty("voice", voices[1].id)
    engine.setProperty("rate", rate)
    engine.setProperty("volume", volume)

    print("🎤 Speaking (English):", text)
    engine.say(text)
    engine.runAndWait()

# ---------- GEMINI TEACHER MODE ----------
def gemini_teacher(prompt):
    teacher_system = (
        "You are Viora, a warm, friendly school teacher who teaches children from KG to Class 8. "
        "Always speak in the SAME language the child is using: Hindi or English. "
        "For KG–2: Use fun tone, very simple words. "
        "For Class 3–5: Use short examples. "
        "For Class 6–8: Clear, simple explanations. "
        "Never use *, •, #, symbols. Just clean text. "
        "TABLE RULE: If asked for table of 2 → 2 ones are 2, 2 twos are 4 ... "
        "HINDI VARNAMALA RULE: If asked → अ आ इ ई उ ऊ ए ऐ ओ औ अं अः "
        "क ख ग घ ङ च छ ज झ ञ ट ठ ड ढ ण त थ द ध न "
        "प फ ब भ म य र ल व श ष स ह. "
        "ENGLISH ABC RULE: Teach as A B C D. in small and in rhythemic manner "
        "If asked silly questions like do you sleep → reply playfully. "
        "Keep all answers short, friendly and natural like a teacher."
        "Never use *, •, #, symbols. Just clean text."
    )

    model = genai.GenerativeModel(
        model_name="gemini-2.0-flash",
        system_instruction=teacher_system
    )

    response = model.generate_content(prompt)
    reply = getattr(response, "text", "I couldn’t process that.")
    return reply[:800]

# WEB SEARCH 
def search_web(query):
    print("🌐 Searching the web for:", query)
    url = "https://google.serper.dev/search"
    headers = {"X-API-KEY": SERPER_API_KEY, "Content-Type": "application/json"}
    payload = {"q": query}

    try:
        response = requests.post(url, headers=headers, json=payload)
        results = response.json()
        if "organic" in results:
            top_results = results["organic"][:3]
            combined_text = " ".join([r.get("snippet", "") for r in top_results])
            return combined_text
        return "I couldn’t find any recent information."
    except:
        return "There was a problem accessing live data."

# CUSTOM RESPONSES 
CUSTOM_RESPONSES = {
    "introduce": "Good Morning! My name is Viora, your friendly AI assistant.",
    "who are you": "I am Viora, your intelligent AI companion.",
    "your name": "My name is Viora.",
    "mentor": "Our mentor and guide is Ankit Jain Sir.",
    "head of department of ec": "The Head of Electronics and Communication Department is Professor Vivek Kumar Rastogi Sir.",
    "coordinator ec 3rd year": "With due respect, the coordinator of EC third year is Gaurav Sahu Sir, who guides our academic journey with great dedication.",
    "second position rocket competition": "With great pride, I’d like to share that Pranveer Singh Institute of Technology, Kanpur secured the second position in the InSpace Model Rocketry India Competition.",
    "best design satellite competition": "The award for Best Design in the CanSat Competition was proudly won by Pranveer Singh Institute of Technology, Kanpur — a remarkable achievement indeed!",
}

def handle_custom_commands(command):
    command_lower = command.lower()
    if "bye" in command_lower or "sleep" in command_lower:
        text_to_speech("Goodbye! I'm going to sleep.")
        exit()
    for key, response in CUSTOM_RESPONSES.items():
        if all(word in command_lower for word in key.split()):
            return response
    return None

# Decision Maker
def decide_engine(command):
    # Teaching questions handled by Gemini Teacher
    teaching_keywords = [
        "table", "alphabet", "alphabets", "varnamala", "hindi", "spell",
        "math", "science", "explain", "teach", "learn"
    ]

    if any(k in command for k in teaching_keywords):
        return gemini_teacher(command)

    # Web-search-based info questions
    info_keywords = ["today", "latest", "now", "2025", "current", "news", "director", "won"]
    if any(k in command for k in info_keywords):
        web_data = search_web(command)
        prompt = f"Explain this simply: {web_data}"
        return gemini_teacher(prompt)

    # Normal conversation
    return gemini_teacher(command)

# Main
def main():
    print("💤 Say 'Viora' to wake me up!")
    while True:
        text = listen_with_google()
        if not text:
            continue
        if "viora" in text:
            text_to_speech("Hey! I’m awake and ready to help you!")
            print("✨ VIORA is active!")

            while True:
                command = listen_with_google()
                if not command:
                    continue
                if "sleep" in command:
                    text_to_speech("Going to sleep now.")
                    exit()

                response = handle_custom_commands(command)
                if response:
                    text_to_speech(response)
                else:
                    ai_response = decide_engine(command)
                    text_to_speech(ai_response)

if __name__ == "__main__":
    main()
