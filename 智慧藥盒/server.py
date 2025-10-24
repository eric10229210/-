from flask import Flask, request, render_template, jsonify
from linebot import LineBotApi, WebhookHandler
from linebot.models import TextSendMessage
from linebot.exceptions import InvalidSignatureError
import os
import time
import threading
from datetime import datetime

app = Flask(__name__)

# LINE Bot 配置
LINE_CHANNEL_ACCESS_TOKEN = os.getenv("LINE_CHANNEL_ACCESS_TOKEN", "oaGZNeDjMl7elifSEGY20KXniMR/Xxg37FPG1fHpzFPBcu+VL8/BfV1KcczOYKtfHCw50kjdqORQsp/98sh2dk1a8q5Ot3slqnNJC8esb2WD0fusNmaFzMh88sWGb3MJxGykxET3WZektW2gvFsoUwdB04t89/1O/w1cDnyilFU=")  # 替換為你的 Token
LINE_CHANNEL_SECRET = os.getenv("LINE_CHANNEL_SECRET", "cce3720584ba1d7a07e44c4502409c10")  # 替換為你的 Secret
LINE_USER_ID = os.getenv("LINE_USER_ID", "Ueb77afb48cfdaa7f27d2ed0a55c13d86")  # 替換為接收者的 LINE User ID
line_bot_api = LineBotApi(LINE_CHANNEL_ACCESS_TOKEN)
handler = WebhookHandler(LINE_CHANNEL_SECRET)


# 主頁路徑，渲染 HTML
# 初始化變數
datav0 = '0'  # 小時
datav1 = '0'  # 分鐘
datav2 = '0'  # 秒
datav3 = '0'  # 溫度
datav4 = '0'  # 濕度
dataled = ' '  # 當前狀態
dataFP = ' '  # 指紋 ID
dataerroll = ' '  # 錯誤狀態
timee = ' '

sent_zero_flag = False  # 初始化標誌變數
# 定時任務的變數
scheduled_hour = None
scheduled_minute = None

# 主頁路徑，渲染 HTML
@app.route('/')
def home():
    appInfo = {
        'hr': datav0,
        'min': datav1,
        'sec': datav2,
        'temp': datav3,
        'humm': datav4,
        'fp_id': dataFP,
        'erroll': dataerroll,
    }
    return render_template('home.html', appInfo=appInfo)


def check_temp_humidity():
    global datav3, datav4
    try:
        temp = float(datav3)  # 將溫度轉換為浮點數
        hum = float(datav4)   # 將濕度轉換為浮點數
        if temp > 27 or hum > 100:
            warning_message = f"警告: 溫度 {temp}°C 或濕度 {hum}% 超過安全範圍！"
            line_bot_api.push_message(LINE_USER_ID, TextSendMessage(text=warning_message))
            print("已發送溫濕度警告至 LINE Bot")
    except ValueError:
        print("無法解析溫濕度數值，跳過檢查")
# 處理感測數據的 POST 請求
@app.route('/reading', methods=['POST'])
def handle_post():
    global datav0, datav1, datav2, datav3, datav4, dataled, dataFP, dataerroll, scheduled_hour, scheduled_minute

    datav0 = request.form.get('hr', '0')   # 預設為 '0'
    datav1 = request.form.get('min', '0')
    datav2 = request.form.get('sec', '0')
    datav3 = request.form.get('temp', '0')
    datav4 = request.form.get('humm', '0')
    new_fp_id = request.form.get('fp_id', '').strip()  # 預設為空字符串
    dataerroll = request.form.get('erroll', ' ')
    print(f"收到數值: hr={datav0}, min={datav1}, sec={datav2}, temp={datav3}, humm={datav4}, fp_id={new_fp_id}, erroll={dataerroll}")

    if new_fp_id is not None and new_fp_id.strip():
        if new_fp_id == '0':  # 如果 ID 是 '0'
            if not sent_zero_flag:  # 檢查是否已經傳送過
                dataFP = new_fp_id
                send_to_line_bot(new_fp_id)  # 傳送到 LINE Bot
                sent_zero_flag = True  # 標誌設為已傳送
        else:  # 如果 ID 不是 '0'
            dataFP = new_fp_id
            send_to_line_bot(new_fp_id)  # 傳送非 0 的 ID
            sent_zero_flag = False  # 重置標誌，允許再次傳送 0

    check_temp_humidity()
    threading.Timer(20, clear_fp_id).start()
    formatted_time = f'{scheduled_hour:02}{scheduled_minute:02}' if scheduled_hour is not None and scheduled_minute is not None else " "

    return f'\r\nled={dataled}{formatted_time}', 200


# 返回最新的感測數據
@app.route('/latest_values', methods=['GET'])
def get_latest_values():
    return jsonify({'hr': datav0, 'min': datav1, 'sec': datav2, 'temp': datav3, 'humm': datav4, 'fp_id': dataFP, 'erroll': dataerroll})

# 用於清空 dataled 的輔助函數
def clear_dataled():
    global dataled
    dataled = ' '
    print('dataled cleared.')
def clear_fp_id():
    global dataFP
    dataFP = ''
    print('fp_id cleared.')
@app.route('/senddata', methods=['POST'])
def senddata():
    global dataled

    try:
        request_data = request.get_json()
        if not request_data or 'data' not in request_data:
            return jsonify({'message': 'Invalid request data'}), 400

        data = request_data.get('data')
        response_message = ''

        # 更新 dataled 狀態
        if data == '0':
            response_message = '註冊'
            dataled = '0'
        elif data == '1':
            response_message = '驗證'
            dataled = '1'
        elif data == '2':
            response_message = '刪除'
            dataled = '2'
        elif data == '3':
            response_message = '已註冊過'
            dataled = '3'
        elif data == '4':
            response_message = '開鎖'
            dataled = '4'
        elif data == '5':
            response_message = '關鎖'
            dataled = '5'
        elif data == '6':
            response_message = '保健'
            dataled = '6'
        else:
            response_message = 'Invalid data, LED reset to space'
            dataled = ' '

        print(f'Received data: {data}, Response: {response_message}')

        # 使用非阻塞計時器清空數據
        threading.Timer(60,clear_dataled ).start()

        return jsonify({'message': response_message, 'led': dataled}), 200

    except Exception as e:
        print(f"處理錯誤: {e}")
        return jsonify({'status': 'error', 'message': str(e)}), 500

# 修改執行間隔的 API
@app.route('/set_interval', methods=['POST'])
def set_interval():
    global execution_interval, timee

    data = request.get_json()
    if not data or 'interval_hours' not in data:
        return jsonify({"error": "請提供 interval_hours"}), 400

    try:
        hours = int(data['interval_hours'])
        if hours < 1:
            return jsonify({"error": "間隔時間必須大於 1 小時"}), 400

        execution_interval = hours * 60 * 60
        timee = datetime.datetime.now() + datetime.timedelta(seconds=execution_interval)
        return jsonify({"message": f"間隔時間已設定為 {hours} 小時"}), 200
    except ValueError:
        return jsonify({"error": "請提供有效的數字"}), 400

# 定時執行的任務
def execute_task():
    global scheduled_hour, scheduled_minute
    print("執行定時任務：啟動馬達")
    try:
        message = TextSendMessage(text="定時任務完成：馬達已啟動")
        line_bot_api.push_message(LINE_USER_ID, message)
        print("已通知 LINE Bot")
    except Exception as e:
        print(f"傳送 LINE Bot 消息失敗：{e}")

    # 等待 10 秒後清除設定時間
    def clear_scheduled_time():
        global scheduled_hour, scheduled_minute
        scheduled_hour = None
        scheduled_minute = None
        print("已清除設定的時間")

    threading.Timer(10, clear_scheduled_time).start()

# 定時任務的排程邏輯
def schedule_task():
    global timee
    while True:
        now = datetime.datetime.now()
        if now >= timee:
            execute_task()
            timee = now + datetime.timedelta(seconds=execution_interval)
            print(f"下一次執行時間：{timee}")
        time.sleep(1)

# 傳送資料到 LINE Bot
@app.route('/callback', methods=['POST'])
def send_to_line_bot(fp_id):
    try:
        message = TextSendMessage(text=f"用戶ID: {fp_id}已成功取藥")
        line_bot_api.push_message(LINE_USER_ID, message)
        print(f"指紋 ID {fp_id} 已傳送到 LINE Bot")
    except Exception as e:
        print(f"無法傳送到 LINE Bot: {e}")

@app.route('/set_time', methods=['POST'])
def set_time():
    global scheduled_hour, scheduled_minute
    data = request.get_json()
    try:
        hours = int(data['hours'])
        minutes = int(data['minutes'])
        if 0 <= hours < 24 and 0 <= minutes < 60:
            scheduled_hour = hours
            scheduled_minute = minutes
            print(f"時間已設定為 {scheduled_hour:02}:{scheduled_minute:02}")
            return jsonify({'message': f'時間已設定為 {scheduled_hour:02}:{scheduled_minute:02}'})
        else:
            return jsonify({'error': '時間超出範圍'}), 400
    except ValueError:
        return jsonify({'error': '無效的時間格式'}), 400



# 啟動伺服器與定時任務
if __name__ == '__main__':
    threading.Thread(target=schedule_task, daemon=True).start()
    app.run(host='172.20.10.4', port=8000, debug=True)