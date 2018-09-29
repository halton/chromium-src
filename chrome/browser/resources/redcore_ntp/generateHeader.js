/**
 * generate Header
 * @param {object} data: title, logo, banner
 */
function generateHeader(data) {
  var defaultData ={
    title: data.title || 'allMobilize 云适配',
    logo: data.logo || './imgs/defaultLogo.png',
    banner: data.banner || './imgs/defaultBanner.png',
  }

  $('#title').text(defaultData.title);
  $('#logo').attr('src', defaultData.logo);
  $('header').css('background-image', "url(" + defaultData.banner + ")");

  // 正式环境
  if (data.id == '5a3b790bf9c071b3be109304') {
      
      // 未登录浏览器人员通告墙
      $.ajax({
          method: "GET",
          url: "http://ysp.redcore.cn/client/v3/user/" + data.id + "/hapless",
          success: function(res) {
              if (res && res.data && res.data.notInstalledArray && (res.data.notInstalledArray.length > 0)) {
                  var userList = res.data.notInstalledArray;
                  var tabNavs = '';
                  userList.forEach(function(items, index) {
                      var nav = "<span title='" + items.departmentName + ", "  + items.email + "'>" + items.name + "</span>";
                      tabNavs += nav;
                  });
				  $('#nav-notice-noRegister').html('');
                  $('#nav-notice-noRegister').append(
                      "<h3>未安装PC端红芯浏览器用户：</h3>" +
                      tabNavs
                  );
              }
              if (res && res.data && res.data.lastLoginArray && (res.data.lastLoginArray.length > 0)) {
                  var userList = res.data.lastLoginArray;
                  var tabNavs = '';
                  userList.forEach(function(items, index) {
                      var nav = "<span title='" + items.departmentName + ", "  + items.email + "'>" + items.name + "</span>";
                      tabNavs += nav;
                  });
					$('#nav-notice-nologin').html('');
                  $('#nav-notice-nologin').append(
                      "<h3>超过三天未使用红芯浏览器用户：</h3>" +
                      tabNavs
                  );
              }
          },
          error: function(err) {
              return;
          }
      });
  }

    // 测试环境环境
    if (data.id == '5ad5622e3a3aa9d98e1d686c') {
        
        // 未登录浏览器人员通告墙
        $.ajax({
            method: "GET",
            url: "http://TG.enterplorer.net/client/v3/user/" + data.id + "/hapless",
            success: function(res) {
                if (res && res.data && res.data.notInstalledArray && (res.data.notInstalledArray.length > 0)) {
                    var userList = res.data.notInstalledArray;
                    var tabNavs = '';
                    userList.forEach(function(items, index) {
                        var nav = "<span title='" + items.departmentName + ", "  + items.email + "'>" + items.name + "</span>";
                        tabNavs += nav;
                    });
					$('#nav-notice-noRegister').html('');
                    $('#nav-notice-noRegister').append(
                        "<h3>未安装PC端红芯浏览器用户：</h3>" +
                        tabNavs
                    );
                }
                if (res && res.data && res.data.lastLoginArray && (res.data.lastLoginArray.length > 0)) {
                    var userList = res.data.lastLoginArray;
                    var tabNavs = '';
                    userList.forEach(function(items, index) {
                        var nav = "<span title='" + items.departmentName + ", "  + items.email + "'>" + items.name + "</span>";
                        tabNavs += nav;
                    });
					$('#nav-notice-nologin').html('');
                    $('#nav-notice-nologin').append(
                        "<h3>超过三天未使用红芯浏览器用户：</h3>" +
                        tabNavs
                    );
                }
            },
            error: function(err) {
                return;
            }
        });
    }

}

var Header = {};
window.Header = Header;

Header.generate = generateHeader;