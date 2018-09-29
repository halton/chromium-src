/**
 * search box
 */
(function search() {
  $searchBox = $('#search-box');
  $searchBtn = $('#search-btn');
  $search = $('#search-input');

  var searchNow = function(words) {
      window.open("https://www.baidu.com/s?wd=" + words, "_self");
  }

  $searchBtn.on('click', function() {
    if($search.val()) {
      searchNow($search.val())
    }
  })

  $search.on('keydown', function(e) {
    var value = $search.val();
    if (e.which == 13 && value) {
      searchNow(value)
    }
  })
})()