import telebot
import serial

BOT_TOKEN = ""
bot = telebot.TeleBot(BOT_TOKEN)

UART_PORT = "/dev/ttyS2"
BAUD_RATE = 115200

try:
	ser = serial.Serial(UART_PORT, BAUD_RATE)
except:
	print(f"ошибка подключения к UART")
	ser = none

@bot.message_handler(commands=['start'])
def start_command(message):
	bot.reply_to(message, "Напиши данные чтобы получить ответ")

@bot.message_handler(func=lambda message: message.text.lower() == 'данные')
def sendUartData(message)
	if ser and ser.is_open
		try:
			data = ser.readline().decode('utf-8').strip()
			if data:
				bot.reply_to(message, f"данные: {data}")
			else:
				bot.reply_to(message, "нет данных")
		except Exception as e:
            bot.reply_to(message, f"Ошибка при чтении UART: {e}")
    else:
       	bot.reply_to(message, "Ошибка: UART не подключен.")

bot.polling()
