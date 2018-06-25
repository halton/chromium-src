<<<<<<< HEAD
/**
 * index.js  项目初始js
 * 功能：初始化三个功能（首页、搜索、二级列表）所需要的模块js
 * 描述：功能较少，所以目前不采用CMD引用模块代码，直接一次引用所需要的模块js代码
 */


var HOMEBROWSER = null ,
    HEADER      = null ,
    GROUP       = null ,
    CARD        = null ,
    HOMEDATA    = null ;

var DATAJS      = "",
    USERINFO    = "";

document.addEventListener("DOMContentLoaded", function() {
    // 初始化模块
    initHomeBrowser();
    initServer();
    initHeader();
    initGroup();
    initHomeData();
    // initCard();
});


// 初始化浏览器端模块
function initHomeBrowser () {
    HOMEBROWSER = new HomeBrowser();
}

// 初始化服务器端模块
function initServer () {
    SERVER = new Server();
}

// 初始化首页面头
function initHeader () {
    HEADER = new Header();
}

// 初始化应用
function initGroup () {
    GROUP = new GroupApp();
}

// 初始化应用组列表
// function initCard () {
//     CARD = new Card();
//     // HOMEBROWSER.getCityFinish();
//     chrome.send("getCity");
// }

// 初始化首页数据入口
function initHomeData () {
    HOMEDATA = new HomeData();
    HEADER.search();
    HOMEDATA.init();
}
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
/**
 * index.js  项目初始js
 * 功能：初始化三个功能（首页、搜索、二级列表）所需要的模块js
 * 描述：功能较少，所以目前不采用CMD引用模块代码，直接一次引用所需要的模块js代码
 */


var HOMEBROWSER = null ,
    HEADER      = null ,
    GROUP       = null ,
    CARD        = null ,
    HOMEDATA    = null ;

var DATAJS      = "",
    USERINFO    = "";

document.addEventListener("DOMContentLoaded", function() {
    // 初始化模块
    initHomeBrowser();
    initServer();
    initHeader();
    initGroup();
    initHomeData();
    // initCard();
});


// 初始化浏览器端模块
function initHomeBrowser () {
    HOMEBROWSER = new HomeBrowser();
}

// 初始化服务器端模块
function initServer () {
    SERVER = new Server();
}

// 初始化首页面头
function initHeader () {
    HEADER = new Header();
}

// 初始化应用
function initGroup () {
    GROUP = new GroupApp();
}

// 初始化应用组列表
// function initCard () {
//     CARD = new Card();
//     // HOMEBROWSER.getCityFinish();
//     chrome.send("getCity");
// }

// 初始化首页数据入口
function initHomeData () {
    HOMEDATA = new HomeData();
    HEADER.search();
    HOMEDATA.init();
}
>>>>>>> ffef05f456bc... Add untracked files after rebase
