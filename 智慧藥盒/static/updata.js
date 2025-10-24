function updateValues() {
    // 向伺服器發送 GET 請求以獲取最新的數值
    $.get('/latest_values', function (data) {
        // 更新感測數據的顯示
        $('#value-hr').text(data.hr || '無資料');
        $('#value-min').text(data.min || '無資料');
        $('#value-sec').text(data.sec || '無資料');
        $('#value-temp').text(data.temp || '無資料');
        $('#value-humm').text(data.humm || '無資料');
        $('#value-fp_id').text(data.fp_id || '無資料');
        $('#value-erroll').text(data.erroll || '無資料');
    }).fail(function () {
        console.error('無法獲取最新數值，請檢查伺服器連接。');
        $('#error-message').text('伺服器連接失敗，請稍後再試。');
    });

    // 向伺服器發送 GET 請求以獲取最新的狀態
    $.get('/get_status', function (data) {
        // 更新狀態訊息
        $('#enroll-status').text(data.message || '尚無狀態');
    }).fail(function () {
        console.error('無法獲取最新狀態，請檢查伺服器連接。');
        $('#enroll-status').text('伺服器連接失敗，請稍後再試。').addClass('error');
    });
}

// 每隔 5 秒更新一次數值和狀態
setInterval(updateValues, 1000);