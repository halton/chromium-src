
/**
 * Home.js  项目初始js
 * 功能：初始化三个功能（首页、搜索、二级列表）所需要的模块js
 * 描述：功能较少，所以目前不采用CMD引用模块代码，直接一次引用所需要的模块js代码
 */


var HOMEPROXY = null;
var LOGIN = null;
var ACCOUNTINFO = null;
var APPGROUPLIST = null;
var deviceId = '';
document.addEventListener("DOMContentLoaded", function () {
    // 初始化模块
    initHomeProxy();
});


// 初始化首页数据入口
function initHomeProxy() {
    HOMEPROXY = new HomeProxy();
    HOMEPROXY.init();
    HOMEPROXY.getFirstLogin();
    HOMEPROXY.getDeviceId();

    HOMEPROXY.registNotification("GET_DEVICEID_NOTIFICATION", this,
        function (data) {
            if (data) {
                deviceId = data;
            }
        });

    HOMEPROXY.registNotification("GET_FIRST_LOGIN_NOTIFICATION", this,
        function (firstLogin) {
            console.log('isFirstLogin: ', firstLogin);
            if (firstLogin) {
                // 如果第一次打开，显示欢迎页
                $('#content').empty();
                $('#content').append('<div class="hello" id="hello" > <section class="section-swiper"> <div class="swiper-container swiper-container-banner"> <div class="swiper-wrapper"> <div id="swiper1" class="swiper-slide swiper-slide-1"> <div class="title-header"> <img src="./hello/img/logo.png" alt="" class="logo"> </div> <div class="title"> <h3> 为办公而生 </h3> <ul class="signin"> <li><a class="helloLoginBtn">登录</a></li> </ul> </div> <div class="page1-main-1"> <ul> <li><img src="./hello/img/swiper1/4.png"></li> <li><img src="./hello/img/swiper1/3.png"></li> <li><img src="./hello/img/swiper1/2.png"></li> <li><img src="./hello/img/swiper1/1.png"></li> <li><img src="./hello/img/swiper1/001.png"></li> </ul> </div> <div class="page1-main-2"> <img src="./hello/img/swiper1/19.png" alt=""> </div> <div class="light"> <img src="./hello/img/swiper1/888.png" alt=""> </div> <div  class="round"> <img class="page1_round" src="./hello/img/round.png" style="display: block;"> </div> <div class="global_scroll" style="opacity: 1;"> <img src="./hello/img/up.png"> </div> </div> <div id="swiper2" class="swiper-slide swiper-slide-2"> <!--左侧注册框--> <div class="register-logon"> <h3> 有身份的浏览器 </h3> <ul class="signin"> <!--<li><a href="https://www.redcore.cn/signup/index.html" target= _blank>注册</a></li>--> <li><a class="helloLoginBtn">登录</a></li> </ul> </div> <!--右侧动画--> <div class="page2"> <div class="page2-main-1"> <img class="page2-img-1" src="./hello/img/swiper2/1.png" style="display: block;"> <img class="page2-img-2" src="./hello/img/swiper2/2.png" style="display: block;"> <img class="page2-img-3" src="./hello/img/swiper2/3.png" style="display: block;"> <img class="page2-img-4" src="./hello/img/swiper2/4.png" style="display: block;"> <img class="page2-img-2 page2-img-2-positive" src="./hello/img/swiper2/2-2.png" style="display: block;"> <img class="page2-img-3 page2-img-3-positive" src="./hello/img/swiper2/3-3.png" style="display: block;"> <img class="page2-img-4 page2-img-4-positive" src="./hello/img/swiper2/4-4.png" style="display: block;"> </div> <div class="page2-main-2"> <img class="page2-bg" src="./hello/img/swiper2/5.png" style="display: block;"> </div> <div class="page2-main-3"> <img class="line-round" src="./hello/img/swiper2/6.png" style="display: block;"> </div> </div> <div class="global_scroll" style="opacity: 1;"> <img src="./hello/img/up-red.png"> </div> </div> <div id="swiper3" class="swiper-slide swiper-slide-3"> <!--左侧注册框--> <div class="register-logon"> <h3> 统一的办公入口 </h3> <ul class="signin"> <li><a class="helloLoginBtn">登录</a></li> </ul> </div> <!--右侧动画--> <div class="page3"> <div class="page3-main-1"> <img class="page3-main-img-1" src="./hello/img/swiper3/7.png" style="display: block;"> <div class="page3-img-pack"> <div class="page3-img-hover"> <img class="page3-img-1" src="./hello/img/swiper3/9.png" style="display: block;"> <img class="page3-img-2" src="./hello/img/swiper3/10.png" style="display: block;"> <img class="page3-img-3" src="./hello/img/swiper3/11.png" style="display: block;"> </div> <div class="page3-img-hover"> <img class="page3-img-4" src="./hello/img/swiper3/12.png" style="display: block;"> <img class="page3-img-5" src="./hello/img/swiper3/13.png" style="display: block;"> </div> </div> </div> <div class="page3-main-2"> <img class="page3-bg" src="./hello/img/swiper3/8.png" style="display: block;"> </div> <div class="page3-main-3"> <img class="line-round" src="./hello/img/round.png" style="display: block;"> </div> </div> <div class="global_scroll" style="opacity: 1;"> <img src="./hello/img/up.png"> </div> </div> <div id="swiper4" class="swiper-slide swiper-slide-4"> <!--左侧注册框--> <div class="register-logon"> <h3> 安全的办公平台 </h3> <ul class="signin"> <li><a class="helloLoginBtn">登录</a></li> </ul> </div> <!--右侧动画--> <div class="page4"> <div class="page4-main-1"> <img class="page4-img-1" src="./hello/img/swiper4/15.png" style="display: block;"> <img class="page4-img-2" src="./hello/img/swiper4/16.png" style="display: block;"> <img class="page4-img-3" src="./hello/img/swiper4/16.png" style="display: block;"> <img class="page4-img-4" src="./hello/img/swiper4/16.png" style="display: block;"> </div> <div class="page4-main-2"> <img class="page4-bg" src="./hello/img/swiper4/17.png" style="display: block;"> </div> <div class="page4-main-3"> <img class="line-round" src="./hello/img/swiper2/6.png" style="display: block;"> </div> </div> </div> </div> <div class="swiper-pagination"></div> </div> </section> </div>'); // $('#hello').css('display', 'block');
                var swiper = new Swiper('.swiper-container-banner', {
                    direction: 'vertical',
                    slidesPerView: 1,
                    spaceBetween: 30,
                    mousewheel: true,
                    pagination: {
                        el: '.swiper-pagination',
                        clickable: true,
                    }
                });
				// 绑定事件
				$(".helloLoginBtn").bind("click", function () {
                    $('#content').empty();
					HOMEPROXY.getLoggingStatus();
				});
            } else {
                $('#content').empty();
				// 否则，还走原来的判断登录逻辑
				HOMEPROXY.getLoggingStatus();
			}
        });
    HOMEPROXY.registNotification("LOGGING_STATUS_NOTIFICATION", this,
        function (status) {
            console.log('login status: ', status)
            if (status.status == 100) {
                HOMEPROXY.getAccountInfo();
                HOMEPROXY.getAppGroupList();
                console.log("login is success");

                switchPage(true)
            }
            else if (status.status < 0) {
				if (!($("#login").length > 0)) {
					switchPage(false);
					LOGIN = new Login();
                    HOMEPROXY.getLastLoginInfo();
                    HOMEPROXY.getAutoLogin();
                    HOMEPROXY.getManagerAddress();
				}
                LOGIN && LOGIN.removeLoading();

                if (status.message) {
                    console.log("status : " + status.message);

                    LOGIN && LOGIN.errorTip(status.message);
                }
                else {
                    console.log('Not login')
                }
            }
        });
    HOMEPROXY.registNotification("GET_ACCOUNT_INFO_NOTIFICATION", this,
        function (info) {
            console.log("updateCompanyInfoCache is done.");
            console.log("companyTitle:" + info.company.name);

            Header.generate({
                title: info.company.name,
                logo: "",
                id: info.company.id
            });
            ACCOUNTINFO = info;
            if (APPGROUPLIST && ACCOUNTINFO)
                Tabs.generate(APPGROUPLIST, ACCOUNTINFO, {});
        });
    HOMEPROXY.registNotification("GET_APP_GROUP_LIST_NOTIFICATION", this,
        function (info) {
            APPGROUPLIST = info;
            if (APPGROUPLIST && ACCOUNTINFO)
                Tabs.generate(APPGROUPLIST, ACCOUNTINFO, {});
        });
    HOMEPROXY.registNotification("GET_LAST_LOGIN_INFO_NOTIFICATION", this,
        function (info) {
            console.log("LAST_LOGIN_INFO_FICATION is done.");
            console.log("LOGIN_INFO: " + info);

            LOGIN.setLoginInfo(info)
        });
    HOMEPROXY.registNotification("GET_AUTO_LOGIN_NOTIFICATION", this,
        function (status) {
            console.log("GET_AUTO_LOGIN_NOTIFICATION is done.");
            console.log("AUTO_LOGIN: " + status);

            LOGIN.setAutoLoginStatus(status)
        });
    HOMEPROXY.registNotification("GET_MANAGER_ADDRESS_NOTIFICATION", this,
        function (info) {
            console.log("GET_MANAGER_ADDRESS_NOTIFICATION is done.");
            console.log("MANAGER_ADDRESS: " + info);

            LOGIN.setManagerAddress(info)
        });
    HOMEPROXY.registNotification("LOGOUTED_NOTIFICATION", this,
        function () {
            console.log("current user is logouted.");
            Tabs.destroy();
            HOMEPROXY.getLoggingStatus();
        });
}
