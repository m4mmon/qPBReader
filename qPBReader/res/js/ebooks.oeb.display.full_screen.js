
/*
 Copyright 2012, Kovid Goyal <kovid at kovidgoyal.net>
 Released under the GPLv3 License
*/


(function() {
  var FullScreen, log;

  log = window.calibre_utils.log;

  FullScreen = (function() {

    FullScreen.name = 'FullScreen';

    function FullScreen() {
      if (!this instanceof arguments.callee) {
        throw new Error('FullScreen constructor called as function');
      }
      this.in_full_screen = false;
      this.initial_left_margin = null;
      this.initial_right_margin = null;
    }

    FullScreen.prototype.save_margins = function() {
      var bs;
      bs = document.body.style;
      this.initial_left_margin = bs.marginLeft;
      return this.initial_right_margin = bs.marginRight;
    };

    FullScreen.prototype.on = function(max_text_width, max_text_height, in_paged_mode) {
      var s;
      if (in_paged_mode) {
        window.paged_display.max_col_width = max_text_width;
        window.paged_display.max_col_height = max_text_height;
      } else {
        s = document.body.style;
        s.maxWidth = max_text_width + 'px';
        s.marginLeft = 'auto';
        s.marginRight = 'auto';
      }
      return window.addEventListener('click', this.handle_click, false);
    };

    FullScreen.prototype.off = function(in_paged_mode) {
      var s;
      window.removeEventListener('click', this.handle_click, false);
      if (in_paged_mode) {
        window.paged_display.max_col_width = -1;
        return window.paged_display.max_col_height = -1;
      } else {
        s = document.body.style;
        s.maxWidth = 'none';
        if (this.initial_left_margin !== null) {
          s.marginLeft = this.initial_left_margin;
        }
        if (this.initial_right_margin !== null) {
          return s.marginRight = this.initial_right_margin;
        }
      }
    };

    FullScreen.prototype.handle_click = function(event) {
      var br, res, _ref, _ref1;
      if (((_ref = event.target) !== document.documentElement && _ref !== document.body) || event.button !== 0) {
        return;
      }
      res = null;
      if (window.paged_display.in_paged_mode) {
        res = window.paged_display.click_for_page_turn(event);
      } else {
        br = document.body.getBoundingClientRect();
        if (!((br.left <= (_ref1 = event.clientX) && _ref1 <= br.right))) {
          res = event.clientX < br.left;
        }
      }
      if (res !== null) {
        return window.py_bridge.page_turn_requested(res);
      }
    };

    return FullScreen;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.full_screen = new FullScreen();
  }

}).call(this);
