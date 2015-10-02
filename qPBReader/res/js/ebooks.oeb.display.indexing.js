
/*
 Copyright 2012, Kovid Goyal <kovid@kovidgoyal.net>
 Released under the GPLv3 License
*/


(function() {
  var BookIndexing, viewport_to_document, window_scroll_pos,
    __indexOf = [].indexOf || function(item) { for (var i = 0, l = this.length; i < l; i++) { if (i in this && this[i] === item) return i; } return -1; };

  window_scroll_pos = function(win) {
    var x, y;
    if (win == null) {
      win = window;
    }
    if (typeof win.pageXOffset === 'number') {
      x = win.pageXOffset;
      y = win.pageYOffset;
    } else {
      if (document.body && (document.body.scrollLeft || document.body.scrollTop)) {
        x = document.body.scrollLeft;
        y = document.body.scrollTop;
      } else if (document.documentElement && (document.documentElement.scrollLeft || document.documentElement.scrollTop)) {
        y = document.documentElement.scrollTop;
        x = document.documentElement.scrollLeft;
      }
    }
    return [x, y];
  };

  viewport_to_document = function(x, y, doc) {
    var frame, rect, win, wx, wy, _ref;
    if (doc == null) {
      doc = typeof window !== "undefined" && window !== null ? window.document : void 0;
    }
    while (doc !== window.document) {
      frame = doc.defaultView.frameElement;
      rect = frame.getBoundingClientRect();
      x += rect.left;
      y += rect.top;
      doc = frame.ownerDocument;
    }
    win = doc.defaultView;
    _ref = window_scroll_pos(win), wx = _ref[0], wy = _ref[1];
    x += wx;
    y += wy;
    return [x, y];
  };

  BookIndexing = (function() {

    BookIndexing.name = 'BookIndexing';

    /*
        This class is a namespace to expose indexing functions via the
        window.book_indexing object. The most important functions are:
    
        anchor_positions(): Get the absolute (document co-ordinate system) position
        for elements with the specified id/name attributes.
    */


    function BookIndexing() {
      this.cache = {};
      this.last_check = [null, null];
    }

    BookIndexing.prototype.cache_valid = function(anchors) {
      var a, p, _i, _len;
      if (!anchors) {
        return false;
      }
      for (_i = 0, _len = anchors.length; _i < _len; _i++) {
        a = anchors[_i];
        if (!Object.prototype.hasOwnProperty.call(this.cache, a)) {
          return false;
        }
      }
      for (p in this.cache) {
        if (Object.prototype.hasOwnProperty.call(this.cache, p) && __indexOf.call(anchors, p) < 0) {
          return false;
        }
      }
      return true;
    };

    BookIndexing.prototype.anchor_positions = function(anchors, use_cache) {
      var anchor, ans, body, br, doc_constant, elem, pos, result, _i, _len, _ref;
      if (use_cache == null) {
        use_cache = false;
      }
      body = document.body;
      doc_constant = body.scrollHeight === this.last_check[1] && body.scrollWidth === this.last_check[0];
      if (use_cache && doc_constant && this.cache_valid(anchors)) {
        return this.cache;
      }
      ans = {};
      if (!anchors) {
        return ans;
      }
      for (_i = 0, _len = anchors.length; _i < _len; _i++) {
        anchor = anchors[_i];
        elem = document.getElementById(anchor);
        if (elem === null) {
          try {
            result = document.evaluate(".//*[local-name() = 'a' and @name='" + anchor + "']", body, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null);
            elem = result.singleNodeValue;
          } catch (error) {
            elem = null;
          }
        }
        if (elem === null) {
          pos = [body.scrollWidth + 1000, body.scrollHeight + 1000];
        } else {
          br = elem.getBoundingClientRect();
          pos = viewport_to_document(br.left, br.top, elem.ownerDocument);
        }
        if ((_ref = window.paged_display) != null ? _ref.in_paged_mode : void 0) {
          pos[0] = window.paged_display.column_at(pos[0]);
        }
        ans[anchor] = pos;
      }
      this.cache = ans;
      this.last_check = [body.scrollWidth, body.scrollHeight];
      return ans;
    };

    BookIndexing.prototype.all_links_and_anchors = function() {
      var a, anchors, body, br, geom, href, id, left, links, name, top, _i, _len, _ref, _ref1, _ref2, _ref3;
      body = document.body;
      links = [];
      anchors = {};
      _ref = document.querySelectorAll("body a[href], body [id], body a[name]");
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        a = _ref[_i];
        if ((_ref1 = window.paged_display) != null ? _ref1.in_paged_mode : void 0) {
          geom = window.paged_display.column_location(a);
        } else {
          br = a.getBoundingClientRect();
          _ref2 = viewport_to_document(br.left, br.top, a.ownerDocument), left = _ref2[0], top = _ref2[1];
          geom = {
            'left': left,
            'top': top,
            'width': br.right - br.left,
            'height': br.bottom - br.top
          };
        }
        href = a.getAttribute('href');
        if (href) {
          links.push([href, geom]);
        }
        id = a.getAttribute("id");
        if (id && __indexOf.call(anchors, id) < 0) {
          anchors[id] = geom;
        }
        if ((_ref3 = a.tagName) === 'A' || _ref3 === "a") {
          name = a.getAttribute("name");
          if (name && __indexOf.call(anchors, name) < 0) {
            anchors[name] = geom;
          }
        }
      }
      return {
        'links': links,
        'anchors': anchors
      };
    };

    return BookIndexing;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.book_indexing = new BookIndexing();
  }

}).call(this);

