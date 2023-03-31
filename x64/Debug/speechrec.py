import speech_recognition as sr


def speech_rec():
    recognizer = sr.Recognizer()
    text = ' '
    while text != 'start':
        with sr.Microphone() as source:
            print("Say something please !")
            recognizer.adjust_for_ambient_noise(source)
            audio = recognizer.listen(source)
            print("Audio is ok")

            try:
                text = recognizer.recognize_google(audio)  # 还可以选择不同的数据源，从而用来识别不同的语言
                print("You said : {}".format(text))
                if text == 'start':
                    print('Start executing!')

            except:
                print("Sorry I can't hear you!")



"""
import speech_recognition as sr

r = sr.Recognizer()

test = sr.AudioFile('/Users/jcyan/Downloads/test.wav')

with test as source:
    r.adjust_for_ambient_noise(source, duration=0.2)
    audio = r.record(source)

print(type (audio))

test = r.recognize_google(audio, language='en-US', show_all= False)

print(type(test))
print(test)
"""

"""
r = sr.Recognizer()     # 调用识别器
harvard = sr.AudioFile('/Users/jcyan/Downloads/test.wav')      # 导入语音文件

# 上下文管理器打开文件并读取其内容
with harvard as source:
    all_audio = r.record(source)    # 使用record()从文件中捕获数据

# 查看类型
print(type(all_audio))      # <class 'speech_recognition.AudioData'>

all_text = r.recognize_sphinx(all_audio)    # 识别输出
print(all_text)
"""