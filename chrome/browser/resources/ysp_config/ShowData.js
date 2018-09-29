/*
 * ShowData.js  首页数据模块
 * 功能：向浏览器端、服务器端获取最新数据，之后渲染首页面
 */

var ShowData = function () {};
ShowData.prototype = function () {
    // 保存浏览器登陆状态
    var LOGINSTATUS = 0;

    // 从浏览器得到当前登录状态--浏览器回调方法
    var _getLoggingStatusFinish = function (loggingStatus) {
		LOGINSTATUS = loggingStatus;
        if (loggingStatus === "100") {
            
            // 从浏览器获取configjs数据
            chrome.send("getParam", ["ShowConfig"]);
            // _getShowConfigFinish('config');
        } else {
            // 这种是不登录状态下访问config文件
            $('#show_body').empty();
            var ERHtml = '';
            ERHtml += '<div class="nologined-content">';
            ERHtml += '   <h1>配置信息未能显示</h1>';
            ERHtml += '   <span>请在浏览器客户端登录用户后查看配置信息</span>';
            ERHtml += '<div>';
            $('#show_body').append(ERHtml);
        }
    };

    // 获取登陆状态完成后 调用获取config json的接口
    var _getShowConfigFinish = function (data) {
        var contentData = JSON.stringify(JSON.parse(data), null, 4);
        // 渲染config 页面
        $('#show_body').empty();
        var cjHtml = '';
        cjHtml += '<pre>';
        cjHtml += contentData;
        cjHtml += '<pre>';
        $('#show_body').append(cjHtml);
    };


    return {
        init: function () {
            chrome.send("getParam", ["loggingStatus"]);
            // _getLoggingStatusFinish("1");
        },
        getLoggingStatusFinish   : _getLoggingStatusFinish,  // 设置登录状态完成--浏览器回调方法
        getShowConfigFinish      : _getShowConfigFinish,
    };
} ();
