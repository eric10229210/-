function sendLED(value) {
    const statusMessage = document.getElementById('statusMessage');
    statusMessage.textContent = '正在處理請求...';

    fetch('/senddata', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ data: value }), // 確認這裡傳遞的是正確的數據
    })
    .then(response => response.json())
    .then(data => {
        statusMessage.textContent = data.message; // 顯示伺服器回應訊息
    })
    .catch(error => {
        statusMessage.textContent = '發生錯誤，請稍後再試。';
        console.error('Error:', error);
    });
}