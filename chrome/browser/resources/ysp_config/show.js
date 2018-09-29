/**
 * index.js  项目初始js
 * 功能：初始化三个功能（首页、搜索、二级列表）所需要的模块js
 * 描述：功能较少，所以目前不采用CMD引用模块代码，直接一次引用所需要的模块js代码
 */


var SHOWBROWSER = null ,
    // HEADER      = null ,
    // GROUP       = null ,
    // CARD        = null ,
    SHOWDATA    = null ;

// var DATAJS      = "",
//     USERINFO    = "";

document.addEventListener("DOMContentLoaded", function() {
    // 初始化模块
    initShowBrowser();
    // initServer();
    // initHeader();
    // initGroup();
    initShowData();
    // initCard();
});


// 初始化浏览器端模块
function initShowBrowser () {
    SHOWBROWSER = new ShowBrowser();
}



// 初始化首页数据入口
function initShowData () {
    SHOWDATA = new ShowData();
    SHOWDATA.init();
}
