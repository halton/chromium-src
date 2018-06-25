<<<<<<< HEAD
/**
 * upload avatar
 */

cr.define('options', function() {
  /** @const */ var Page = cr.ui.pageManager.Page;
  /** @const */ var PageManager = cr.ui.pageManager.PageManager;

cr.addSingletonGetter(PersonalDataUpload);

function PersonalDataUpload() {

  Page.call(this, 'upload',
  '头像上传',
  'upload-avatar-section');

  /**
   * toBlob
   * @desc A low performance polyfill based on toDataURL.
   */
  if (!HTMLCanvasElement.prototype.toBlob) {
    Object.defineProperty(HTMLCanvasElement.prototype, 'toBlob', {
      value: function (callback, type, quality) {
        var canvas = this;
        setTimeout(function() {

          var binStr = atob( canvas.toDataURL(type, quality).split(',')[1] ),
              len = binStr.length,
              arr = new Uint8Array(len);

          for (var i = 0; i < len; i++ ) {
            arr[i] = binStr.charCodeAt(i);
          }

          callback( new Blob( [arr], {type: type || 'image/png'} ) );

        });
      }
    });
  }

  /**
   * send blob data to server
   * @param {string} url 
   * @param {obj} options 
   */
  function sendBlob(url, options) {
    var method = options.method;
    var data = options.data;
    var success = options.success;
    var error = options.error;
    var progress = options.progress;

    var upload = new XMLHttpRequest();

    upload.open(method, url, true);
    upload.addEventListener('load', function () {
      success(this.responseText)
    })
    upload.addEventListener('progress', function () {
      progress()
    })
    upload.addEventListener('error', function () {
      error(this.responseText)
    })
    
    upload.send(data);
  }


  var inputImage  = $('upload-avatar-input');
  var image = $('image');
  var preview = $('preview');
  var defaultImg = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABoAAAAgCAMAAAA7dZg3AAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAK7OHOkAAAB+UExURUxpcczMzM3NzczMzOHh4czMzMzMzM3NzdHR0czMzMzMzMzMzM7OzszMzM3Nzc3NzdXV1c3NzdbW1s3NzdTU1MzMzM3NzczMzP///83NzczMzMzMzMzMzMzMzM3NzczMzM3Nzc/Pz8zMzM3Nzc7OztDQ0MzMzM3NzczMzMzMzE2VTu8AAAApdFJOUwDadeYE98eFGP65rDzgv30JiQ5oEfNM7wKUeaai+zNBSCOy0FEsl1hHIaHB9gAAARhJREFUeNptkVmOgzAUBM1ijNlDQkiA7NvU/S84GoLBzqS/Wiqp3E8WJufBk7q9lZX4SFwwJdrWDtl4EAylrxKg3dqoR5fh2JQGeVzIA8pZkAXcdjNaEYRiGQR3YRKxt/UF17kfeNloTWtqDZmNfsD4YzjaaIs2tQLnFJ987h7KRimptTCxUbSgULN2hY2pu8Mnauee4NUL2QTWWyfoF5SgrVtW8DA9g86yhwHPZbon7Chk/G6XA76DYklUj+VGsxFOfFDTq2vhpgpYvc155ei6qIHztK+JutjMUxIwH70HkCocVQnoQp3m+1WhIfnT9pBchJPLFfpR/nTBpM3EgBf+R6HHIHJK8SUduZDcv6E7UqR5/A3FefoL4ZYajPpmOsYAAAAASUVORK5CYII=';

  image.src = defaultImg;
  preview.src = defaultImg;

  var options = {
    aspectRatio: 1 / 1,
    preview: document.querySelector('#preview-wrap'),
    rotatable: false,
    zoomOnTouch: false,
    minContainerWidth: 160
  };
  var fileFormat = '.jpg, .jpeg, .png';
  var cropper = null;

  inputImage.setAttribute('accept', fileFormat);


  /**
   * select img from Local && instance cropper
   */
  if (window.URL) {
    inputImage.onchange = function () {
      var files = this.files;
      var file;
      console.log(files)

      if (files && files.length) {
        file = files[0];
        console.log(file)

        if (file.size > 1024 * 1024) {
          console.log('too big')
          inputImage.value = null; 
          return;
        }

        if (/^image\/\w+/.test(file.type)) {

          image.src = URL.createObjectURL(file);
          cropper && cropper.destroy();

          cropper = new Cropper(image, options);
          inputImage.value = null;
        } else {
          console.log('Please choose an image file.');
        }
      }
    };
  } else {
    inputImage.disabled = true;
  }

  /**
   * crop img && upload to server
   * make sure cropper is here
   */
  $('upload-avatar-conform').addEventListener('click', function() {
    cropper.getCroppedCanvas({
      width: 90,
      height: 90,
      minWidth: 256,
      minHeight: 256,
      maxWidth: 596,
      maxHeight: 596,
      fillColor: '#fff',
      imageSmoothingEnabled: false,
      imageSmoothingQuality: 'high',
    });

    cropper.getCroppedCanvas().toBlob(function (blob) {
      var formData = new FormData();
      
      formData.append('file', blob);
      formData.append('userid', '')
      formData.append('companyid', '')
    
      console.log(blob, formData)
    
      /**
       * send data to server
       */
      // sendBlob('', {
      //   method: 'POST',
      //   data: formData,
      //   success: function(data) {
      //     console.log(data);
      //     // chrome.send('avatar', [url])
      //     PageManager.closeOverlay();
      //   }
      // })
    });
  })


  $('upload-avatar-cancel').addEventListener('click', function() {
    // chrome.send('close')
    PageManager.closeOverlay();
  })

}

PersonalDataUpload.prototype = {
  __proto__: Page.prototype,
}

return {
  PersonalDataUpload:  PersonalDataUpload,
}

})
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
/**
 * upload avatar
 */

cr.define('options', function() {
  /** @const */ var Page = cr.ui.pageManager.Page;
  /** @const */ var PageManager = cr.ui.pageManager.PageManager;

cr.addSingletonGetter(PersonalDataUpload);

function PersonalDataUpload() {

  Page.call(this, 'upload',
  '头像上传',
  'upload-avatar-section');

  /**
   * toBlob
   * @desc A low performance polyfill based on toDataURL.
   */
  if (!HTMLCanvasElement.prototype.toBlob) {
    Object.defineProperty(HTMLCanvasElement.prototype, 'toBlob', {
      value: function (callback, type, quality) {
        var canvas = this;
        setTimeout(function() {

          var binStr = atob( canvas.toDataURL(type, quality).split(',')[1] ),
              len = binStr.length,
              arr = new Uint8Array(len);

          for (var i = 0; i < len; i++ ) {
            arr[i] = binStr.charCodeAt(i);
          }

          callback( new Blob( [arr], {type: type || 'image/png'} ) );

        });
      }
    });
  }

  /**
   * send blob data to server
   * @param {string} url 
   * @param {obj} options 
   */
  function sendBlob(url, options) {
    var method = options.method;
    var data = options.data;
    var success = options.success;
    var error = options.error;
    var progress = options.progress;

    var upload = new XMLHttpRequest();

    upload.open(method, url, true);
    upload.addEventListener('load', function () {
      success(this.responseText)
    })
    upload.addEventListener('progress', function () {
      progress()
    })
    upload.addEventListener('error', function () {
      error(this.responseText)
    })
    
    upload.send(data);
  }


  var inputImage  = $('upload-avatar-input');
  var image = $('image');
  var preview = $('preview');
  var defaultImg = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABoAAAAgCAMAAAA7dZg3AAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAK7OHOkAAAB+UExURUxpcczMzM3NzczMzOHh4czMzMzMzM3NzdHR0czMzMzMzMzMzM7OzszMzM3Nzc3NzdXV1c3NzdbW1s3NzdTU1MzMzM3NzczMzP///83NzczMzMzMzMzMzMzMzM3NzczMzM3Nzc/Pz8zMzM3Nzc7OztDQ0MzMzM3NzczMzMzMzE2VTu8AAAApdFJOUwDadeYE98eFGP65rDzgv30JiQ5oEfNM7wKUeaai+zNBSCOy0FEsl1hHIaHB9gAAARhJREFUeNptkVmOgzAUBM1ijNlDQkiA7NvU/S84GoLBzqS/Wiqp3E8WJufBk7q9lZX4SFwwJdrWDtl4EAylrxKg3dqoR5fh2JQGeVzIA8pZkAXcdjNaEYRiGQR3YRKxt/UF17kfeNloTWtqDZmNfsD4YzjaaIs2tQLnFJ987h7KRimptTCxUbSgULN2hY2pu8Mnauee4NUL2QTWWyfoF5SgrVtW8DA9g86yhwHPZbon7Chk/G6XA76DYklUj+VGsxFOfFDTq2vhpgpYvc155ei6qIHztK+JutjMUxIwH70HkCocVQnoQp3m+1WhIfnT9pBchJPLFfpR/nTBpM3EgBf+R6HHIHJK8SUduZDcv6E7UqR5/A3FefoL4ZYajPpmOsYAAAAASUVORK5CYII=';

  image.src = defaultImg;
  preview.src = defaultImg;

  var options = {
    aspectRatio: 1 / 1,
    preview: document.querySelector('#preview-wrap'),
    rotatable: false,
    zoomOnTouch: false,
    minContainerWidth: 160
  };
  var fileFormat = '.jpg, .jpeg, .png';
  var cropper = null;

  inputImage.setAttribute('accept', fileFormat);


  /**
   * select img from Local && instance cropper
   */
  if (window.URL) {
    inputImage.onchange = function () {
      var files = this.files;
      var file;
      console.log(files)

      if (files && files.length) {
        file = files[0];
        console.log(file)

        if (file.size > 1024 * 1024) {
          console.log('too big')
          inputImage.value = null; 
          return;
        }

        if (/^image\/\w+/.test(file.type)) {

          image.src = URL.createObjectURL(file);
          cropper && cropper.destroy();

          cropper = new Cropper(image, options);
          inputImage.value = null;
        } else {
          console.log('Please choose an image file.');
        }
      }
    };
  } else {
    inputImage.disabled = true;
  }

  /**
   * crop img && upload to server
   * make sure cropper is here
   */
  $('upload-avatar-conform').addEventListener('click', function() {
    cropper.getCroppedCanvas({
      width: 90,
      height: 90,
      minWidth: 256,
      minHeight: 256,
      maxWidth: 596,
      maxHeight: 596,
      fillColor: '#fff',
      imageSmoothingEnabled: false,
      imageSmoothingQuality: 'high',
    });

    cropper.getCroppedCanvas().toBlob(function (blob) {
      var formData = new FormData();
      
      formData.append('file', blob);
      formData.append('userid', '')
      formData.append('companyid', '')
    
      console.log(blob, formData)
    
      /**
       * send data to server
       */
      // sendBlob('', {
      //   method: 'POST',
      //   data: formData,
      //   success: function(data) {
      //     console.log(data);
      //     // chrome.send('avatar', [url])
      //     PageManager.closeOverlay();
      //   }
      // })
    });
  })


  $('upload-avatar-cancel').addEventListener('click', function() {
    // chrome.send('close')
    PageManager.closeOverlay();
  })

}

PersonalDataUpload.prototype = {
  __proto__: Page.prototype,
}

return {
  PersonalDataUpload:  PersonalDataUpload,
}

})
>>>>>>> ffef05f456bc... Add untracked files after rebase
