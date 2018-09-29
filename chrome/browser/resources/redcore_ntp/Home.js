
/**
 * Home.js  项目初始js
 * 功能：初始化三个功能（首页、搜索、二级列表）所需要的模块js
 * 描述：功能较少，所以目前不采用CMD引用模块代码，直接一次引用所需要的模块js代码
 */


var HOMEPROXY = null;
var LOGIN = null;
var isInitLogin = false;
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
                $('#hello').css('display', 'block');
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
                    $('#hello').css('display', 'none');
					HOMEPROXY.getLoggingStatus();
				});
            } else {
                $('#hello').css('display', 'none');
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
                switchPage(false);
                LOGIN && LOGIN.removeLoading();

                if (!isInitLogin) {
                    LOGIN = new Login();

                    HOMEPROXY.getLastLoginInfo();
                    HOMEPROXY.getAutoLogin();
                    HOMEPROXY.getManagerAddress();

                    isInitLogin = true;
                }

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
