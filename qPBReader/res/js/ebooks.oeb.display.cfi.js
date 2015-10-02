
/*
 Copyright 2011, Kovid Goyal <kovid@kovidgoyal.net>
 Released under the GPLv3 License
 Based on code originally written by Peter Sorotkin
 http://code.google.com/p/epub-revision/source/browse/trunk/src/samples/cfi/epubcfi.js

 Improvements with respect to that code:
 1. Works on all browsers (WebKit, Firefox and IE >= 9)
 2. Works for content in elements that are scrollable (i.e. have their own scrollbars)
 3. Much more comprehensive testing/error handling
 4. Properly encodes/decodes assertions
 5. Handles points in the padding of elements consistently
 6. Has a utility method to calculate the CFI for the current viewport position robustly

 To check if this script is compatible with the current browser, call
 window.cfi.is_compatible() it will throw an exception if not compatible.

 Tested on: Firefox 9, IE 9, Chromium 16, Qt WebKit 2.1
*/


(function() {
  var CanonicalFragmentIdentifier, cfi, escape_for_cfi, find_offset_for_point, fstr, get_current_time, log, offset_in_text_node, range_has_point, t, unescape_from_cfi, viewport_to_document, window_scroll_pos;

  log = function(error) {
    var _ref, _ref1;
    if (error) {
      if (typeof window !== "undefined" && window !== null ? (_ref = window.console) != null ? _ref.log : void 0 : void 0) {
        return window.console.log(error);
      } else if (typeof process !== "undefined" && process !== null ? (_ref1 = process.stdout) != null ? _ref1.write : void 0 : void 0) {
        return process.stdout.write(error + '\n');
      }
    }
  };

  escape_for_cfi = function(raw) {
    var c, _i, _len, _ref;
    if (raw) {
      _ref = ['^', '[', ']', ',', '(', ')', ';', '~', '@', '-', '!'];
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        c = _ref[_i];
        raw = raw.replace(c, '^' + c);
      }
    }
    return raw;
  };

  unescape_from_cfi = function(raw) {
    var ans, c, dropped, _i, _len;
    ans = raw;
    if (raw) {
      dropped = false;
      ans = [];
      for (_i = 0, _len = raw.length; _i < _len; _i++) {
        c = raw[_i];
        if (!dropped && c === '^') {
          dropped = true;
          continue;
        }
        dropped = false;
        ans.push(c);
      }
      ans = ans.join('');
    }
    return ans;
  };

  fstr = function(d) {
    var ans, n;
    ans = "";
    if (d < 0) {
      ans = "-";
      d = -d;
    }
    n = Math.floor(d);
    ans += n;
    n = Math.round((d - n) * 100);
    if (n !== 0) {
      ans += ".";
      ans += n % 10 === 0 ? n / 10 : n;
    }
    return ans;
  };

  get_current_time = function(target) {
    var ans;
    ans = 0;
    if (target.currentTime !== void 0) {
      ans = target.currentTime;
    }
    return fstr(ans);
  };

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

  range_has_point = function(range, x, y) {
    var rect, _i, _len, _ref;
    _ref = range.getClientRects();
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      rect = _ref[_i];
      if (((rect.left <= x && x <= rect.right)) && ((rect.top <= y && y <= rect.bottom))) {
        return true;
      }
    }
    return false;
  };

  offset_in_text_node = function(node, range, x, y) {
    var limits, lr, pivot, rr;
    limits = [0, node.nodeValue.length];
    while (limits[0] !== limits[1]) {
      pivot = Math.floor((limits[0] + limits[1]) / 2);
      lr = [limits[0], pivot];
      rr = [pivot + 1, limits[1]];
      range.setStart(node, pivot);
      range.setEnd(node, pivot + 1);
      if (range_has_point(range, x, y)) {
        return pivot;
      }
      range.setStart(node, rr[0]);
      range.setEnd(node, rr[1]);
      if (range_has_point(range, x, y)) {
        limits = rr;
        continue;
      }
      range.setStart(node, lr[0]);
      range.setEnd(node, lr[1]);
      if (range_has_point(range, x, y)) {
        limits = lr;
        continue;
      }
      break;
    }
    return limits[0];
  };

  find_offset_for_point = function(x, y, node, cdoc) {
    var child, range, _ref, _ref1;
    range = cdoc.createRange();
    child = node.firstChild;
    while (child) {
      if (((_ref = child.nodeType) === 3 || _ref === 4 || _ref === 5 || _ref === 6) && ((_ref1 = child.nodeValue) != null ? _ref1.length : void 0)) {
        range.setStart(child, 0);
        range.setEnd(child, child.nodeValue.length);
        if (range_has_point(range, x, y)) {
          return [child, offset_in_text_node(child, range, x, y)];
        }
      }
      child = child.nextSibling;
    }
    throw "Point (" + x + ", " + y + ") is in the padding/border of " + node + ", so cannot calculate offset";
  };

  CanonicalFragmentIdentifier = (function() {

    CanonicalFragmentIdentifier.name = 'CanonicalFragmentIdentifier';

    /*
        This class is a namespace to expose CFI functions via the window.cfi
        object. The most important functions are:
    
        is_compatible(): Throws an error if the browser is not compatible with
                         this script
    
        at(x, y): Maps a point to a CFI, if possible
        at_current(): Returns the CFI corresponding to the current viewport scroll location
    
        scroll_to(cfi): which scrolls the browser to a point corresponding to the
                        given cfi, and returns the x and y co-ordinates of the point.
    */


    function CanonicalFragmentIdentifier() {
      var div, ver;
      if (!this instanceof arguments.callee) {
        throw new Error('CFI constructor called as function');
      }
      this.CREATE_RANGE_ERR = "Your browser does not support the createRange function. Update it to a newer version.";
      this.IE_ERR = "Your browser is too old. You need Internet Explorer version 9 or newer.";
      div = document.createElement('div');
      ver = 3;
      while (true) {
        div.innerHTML = "<!--[if gt IE " + (++ver) + "]><i></i><![endif]-->";
        if (div.getElementsByTagName('i').length === 0) {
          break;
        }
      }
      this.iever = ver;
      this.isie = ver > 4;
    }

    CanonicalFragmentIdentifier.prototype.is_compatible = function() {
      if (!window.document.createRange) {
        throw this.CREATE_RANGE_ERR;
      }
      if (this.isie && this.iever < 9) {
        throw this.IE_ERR;
      }
    };

    CanonicalFragmentIdentifier.prototype.set_current_time = function(target, val) {
      var fn;
      if (target.currentTime === void 0) {
        return;
      }
      if (target.readyState === 4 || target.readyState === "complete") {
        return target.currentTime = val + 0;
      } else {
        fn = function() {
          return target.currentTime = val;
        };
        return target.addEventListener("canplay", fn, false);
      }
    };

    CanonicalFragmentIdentifier.prototype.encode = function(doc, node, offset, tail) {
      var cfi, child, id, idspec, index, p, win, _ref, _ref1;
      cfi = tail || "";
      switch (node.nodeType) {
        case 1:
          if (typeof offset === 'number') {
            node = node.childNodes.item(offset);
          }
          break;
        case 3:
        case 4:
        case 5:
        case 6:
          offset || (offset = 0);
          while (true) {
            p = node.previousSibling;
            if (!p || p.nodeType > 8) {
              break;
            }
            if (((_ref = p.nodeType) !== 2 && _ref !== 8) && (((_ref1 = p.nodeValue) != null ? _ref1.length : void 0) != null)) {
              offset += p.nodeValue.length;
            }
            node = p;
          }
          cfi = ":" + offset + cfi;
          break;
        default:
          log("Offsets for nodes of type " + node.nodeType + " are not handled");
      }
      while (node !== doc) {
        p = node.parentNode;
        if (!p) {
          if (node.nodeType === 9) {
            win = node.defaultView;
            if (win.frameElement) {
              node = win.frameElement;
              cfi = "!" + cfi;
              continue;
            }
          }
          break;
        }
        index = 0;
        child = p.firstChild;
        while (true) {
          index |= 1;
          if (child.nodeType === 1) {
            index++;
          }
          if (child === node) {
            break;
          }
          child = child.nextSibling;
        }
        id = typeof node.getAttribute === "function" ? node.getAttribute('id') : void 0;
        idspec = id ? "[" + (escape_for_cfi(id)) + "]" : '';
        cfi = '/' + index + idspec + cfi;
        node = p;
      }
      return cfi;
    };

    CanonicalFragmentIdentifier.prototype.decode = function(cfi, doc) {
      var assertion, child, error, index, len, next, nn, node, offset, point, r, simple_node_regex, target, _ref, _ref1, _ref2;
      if (doc == null) {
        doc = typeof window !== "undefined" && window !== null ? window.document : void 0;
      }
      simple_node_regex = /^\/(\d+)(\[[^\]]*\])?/;
      error = null;
      node = doc;
      while (!(cfi.length < 1 || error)) {
        if ((r = cfi.match(simple_node_regex))) {
          target = parseInt(r[1]);
          assertion = r[2];
          if (assertion) {
            assertion = unescape_from_cfi(assertion.slice(1, assertion.length - 1));
          }
          index = 0;
          child = node.firstChild;
          while (true) {
            if (!child) {
              if (assertion) {
                child = doc.getElementById(assertion);
                if (child) {
                  node = child;
                }
              }
              if (!child) {
                error = "No matching child found for CFI: " + cfi;
              }
              cfi = cfi.substr(r[0].length);
              break;
            }
            index |= 1;
            if (child.nodeType === 1) {
              index++;
            }
            if (index === target) {
              cfi = cfi.substr(r[0].length);
              node = child;
              if (assertion && node.id !== assertion) {
                child = doc.getElementById(assertion);
                if (child) {
                  node = child;
                }
              }
              break;
            }
            child = child.nextSibling;
          }
        } else if (cfi[0] === '!') {
          if (node.contentDocument) {
            node = node.contentDocument;
            cfi = cfi.substr(1);
          } else {
            error = ("Cannot reference " + node.nodeName + "'s content:") + cfi;
          }
        } else {
          break;
        }
      }
      if (error) {
        log(error);
        return null;
      }
      point = {};
      error = null;
      offset = null;
      if ((r = cfi.match(/^:(\d+)/)) !== null) {
        offset = parseInt(r[1]);
        cfi = cfi.substr(r[0].length);
      }
      if ((r = cfi.match(/^~(-?\d+(\.\d+)?)/)) !== null) {
        point.time = r[1] - 0;
        cfi = cfi.substr(r[0].length);
      }
      if ((r = cfi.match(/^@(-?\d+(\.\d+)?):(-?\d+(\.\d+)?)/)) !== null) {
        point.x = r[1] - 0;
        point.y = r[3] - 0;
        cfi = cfi.substr(r[0].length);
      }
      if ((r = cfi.match(/^\[([^\]]+)\]/)) !== null) {
        assertion = r[1];
        cfi = cfi.substr(r[0].length);
        if ((r = assertion.match(/;s=([ab])$/)) !== null) {
          if (r.index > 0 && assertion[r.index - 1] !== '^') {
            assertion = assertion.substr(0, r.index);
            point.forward = r[1] === 'a';
          }
          assertion = unescape_from_cfi(assertion);
        }
      }
      if (node != null) {
        if ((_ref = node.parentNode) != null) {
          _ref.normalize();
        }
      }
      if (offset !== null) {
        while (true) {
          len = node.nodeValue.length;
          if (offset < len || (!point.forward && offset === len)) {
            break;
          }
          next = false;
          while (true) {
            nn = node.nextSibling;
            if (!nn) {
              break;
            }
            if (((_ref1 = nn.nodeType) === 3 || _ref1 === 4 || _ref1 === 5 || _ref1 === 6) && ((_ref2 = nn.nodeValue) != null ? _ref2.length : void 0)) {
              next = nn;
              break;
            }
            node = nn;
          }
          if (!next) {
            if (offset > len) {
              error = "Offset out of range: " + offset;
              offset = len;
            }
            break;
          }
          node = next;
          offset -= len;
        }
        point.offset = offset;
      }
      point.node = node;
      if (error) {
        point.error = error;
      } else if (cfi.length > 0) {
        point.error = "Undecoded CFI: " + cfi;
      }
      log(point.error);
      return point;
    };

    CanonicalFragmentIdentifier.prototype.at = function(x, y, doc) {
      var cd, cdoc, cwin, name, offset, px, py, rect, tail, target, _ref, _ref1;
      if (doc == null) {
        doc = typeof window !== "undefined" && window !== null ? window.document : void 0;
      }
      cdoc = doc;
      target = null;
      cwin = cdoc.defaultView;
      tail = '';
      offset = null;
      name = null;
      while (true) {
        target = cdoc.elementFromPoint(x, y);
        if (!target || ((_ref = target.localName) === 'html' || _ref === 'body')) {
          return null;
        }
        name = target.localName;
        if (name !== 'iframe' && name !== 'embed' && name !== 'object') {
          break;
        }
        cd = target.contentDocument;
        if (!cd) {
          break;
        }
        rect = target.getBoundingClientRect();
        x -= rect.left;
        y -= rect.top;
        cdoc = cd;
        cwin = cdoc.defaultView;
      }
      (target.parentNode ? target.parentNode : target).normalize();
      if (name === 'audio' || name === 'video') {
        tail = "~" + get_current_time(target);
      }
      if (name === 'img' || name === 'video') {
        rect = target.getBoundingClientRect();
        px = ((x - rect.left) * 100) / target.offsetWidth;
        py = ((y - rect.top) * 100) / target.offsetHeight;
        tail = "" + tail + "@" + (fstr(px)) + ":" + (fstr(py));
      } else if (name !== 'audio') {
        if (cdoc.createRange) {
          _ref1 = find_offset_for_point(x, y, target, cdoc), target = _ref1[0], offset = _ref1[1];
        } else {
          throw this.CREATE_RANGE_ERR;
        }
      }
      return this.encode(doc, target, offset, tail);
    };

    CanonicalFragmentIdentifier.prototype.point = function(cfi, doc) {
      var a, end_offset, i, k, ndoc, node, node_len, nwin, offset, r, range, rects, start_offset, t, try_list, x, y, _i, _len, _ref, _ref1;
      if (doc == null) {
        doc = typeof window !== "undefined" && window !== null ? window.document : void 0;
      }
      r = this.decode(cfi, doc);
      if (!r) {
        return null;
      }
      node = r.node;
      ndoc = node.ownerDocument;
      if (!ndoc) {
        log("CFI node has no owner document: " + cfi + " " + node);
        return null;
      }
      nwin = ndoc.defaultView;
      x = null;
      y = null;
      range = null;
      if (typeof r.offset === "number") {
        if (!ndoc.createRange) {
          throw this.CREATE_RANGE_ERR;
        }
        range = ndoc.createRange();
        if (r.forward) {
          try_list = [
            {
              start: 0,
              end: 0,
              a: 0.5
            }, {
              start: 0,
              end: 1,
              a: 1
            }, {
              start: -1,
              end: 0,
              a: 0
            }
          ];
        } else {
          try_list = [
            {
              start: 0,
              end: 0,
              a: 0.5
            }, {
              start: -1,
              end: 0,
              a: 0
            }, {
              start: 0,
              end: 1,
              a: 1
            }
          ];
        }
        a = null;
        rects = null;
        node_len = node.nodeValue.length;
        offset = r.offset;
        _ref = [0, 1];
        for (_i = 0, _len = _ref.length; _i < _len; _i++) {
          i = _ref[_i];
          offset = r.offset - i;
          if (offset < 0) {
            offset = 0;
          }
          k = 0;
          while (!((rects != null ? rects.length : void 0) || k >= try_list.length)) {
            t = try_list[k++];
            start_offset = offset + t.start;
            end_offset = offset + t.end;
            a = t.a;
            if (start_offset < 0 || end_offset >= node_len) {
              continue;
            }
            range.setStart(node, start_offset);
            range.setEnd(node, end_offset);
            rects = range.getClientRects();
          }
          if (rects != null ? rects.length : void 0) {
            break;
          }
        }
        if (!(rects != null ? rects.length : void 0)) {
          log("Could not find caret position: rects: " + rects + " offset: " + r.offset);
          return null;
        }
      } else {
        _ref1 = [r.x, r.y], x = _ref1[0], y = _ref1[1];
      }
      return {
        x: x,
        y: y,
        node: r.node,
        time: r.time,
        range: range,
        a: a
      };
    };

    CanonicalFragmentIdentifier.prototype.scroll_to = function(cfi, callback, doc) {
      var ec, eo, fn, ndoc, node, nwin, point, r, sc, so, span, _ref, _ref1, _ref2;
      if (callback == null) {
        callback = false;
      }
      if (doc == null) {
        doc = typeof window !== "undefined" && window !== null ? window.document : void 0;
      }
      if (((_ref = window.mathjax) != null ? _ref.math_present : void 0) && !((_ref1 = window.mathjax) != null ? _ref1.math_loaded : void 0)) {
        window.mathjax.pending_cfi = [cfi, callback];
        return;
      }
      point = this.point(cfi, doc);
      if (!point) {
        log("No point found for cfi: " + cfi);
        return;
      }
      if (typeof point.time === 'number') {
        this.set_current_time(point.node, point.time);
      }
      if (point.range !== null) {
        r = point.range;
        _ref2 = [r.startOffset, r.endOffset, r.startContainer, r.endContainer], so = _ref2[0], eo = _ref2[1], sc = _ref2[2], ec = _ref2[3];
        node = r.startContainer;
        ndoc = node.ownerDocument;
        nwin = ndoc.defaultView;
        span = ndoc.createElement('span');
        span.setAttribute('style', 'border-width: 0; padding: 0; margin: 0');
        r.surroundContents(span);
        span.scrollIntoView();
        fn = function() {
          var node, p, rect, rects, x, y, _i, _len, _ref3, _ref4;
          rect = span.getBoundingClientRect();
          p = span.parentNode;
          _ref3 = span.childNodes;
          for (_i = 0, _len = _ref3.length; _i < _len; _i++) {
            node = _ref3[_i];
            span.removeChild(node);
            p.insertBefore(node, span);
          }
          p.removeChild(span);
          p.normalize();
          r.setStart(sc, so);
          r.setEnd(ec, eo);
          rects = r.getClientRects();
          if (rects.length > 0) {
            rect = rects[0];
          }
          x = point.a * rect.left + (1 - point.a) * rect.right;
          y = (rect.top + rect.bottom) / 2;
          _ref4 = viewport_to_document(x, y, ndoc), x = _ref4[0], y = _ref4[1];
          if (callback) {
            return callback(x, y);
          }
        };
      } else {
        node = point.node;
        nwin = node.ownerDocument.defaultView;
        node.scrollIntoView();
        fn = function() {
          var x, y, _ref3;
          r = node.getBoundingClientRect();
          _ref3 = viewport_to_document(r.left, r.top, node.ownerDocument), x = _ref3[0], y = _ref3[1];
          if (typeof point.x === 'number' && node.offsetWidth) {
            x += (point.x * node.offsetWidth) / 100;
          }
          if (typeof point.y === 'number' && node.offsetHeight) {
            y += (point.y * node.offsetHeight) / 100;
          }
          scrollTo(x, y);
          if (callback) {
            return callback(x, y);
          }
        };
      }
      setTimeout(fn, 10);
      return null;
    };

    CanonicalFragmentIdentifier.prototype.at_point = function(ox, oy) {
      var cfi, dist, node, point, r, rect, x, y, _ref, _ref1;
      dist = function(p1, p2) {
        return Math.sqrt(Math.pow(p1[0] - p2[0], 2), Math.pow(p1[1] - p2[1], 2));
      };
      try {
        cfi = window.cfi.at(ox, oy);
        point = window.cfi.point(cfi);
      } catch (err) {
        cfi = null;
      }
      if (cfi) {
        if (point.range !== null) {
          r = point.range;
          rect = r.getClientRects()[0];
          x = point.a * rect.left + (1 - point.a) * rect.right;
          y = (rect.top + rect.bottom) / 2;
          _ref = viewport_to_document(x, y, r.startContainer.ownerDocument), x = _ref[0], y = _ref[1];
        } else {
          node = point.node;
          r = node.getBoundingClientRect();
          _ref1 = viewport_to_document(r.left, r.top, node.ownerDocument), x = _ref1[0], y = _ref1[1];
          if (typeof point.x === 'number' && node.offsetWidth) {
            x += (point.x * node.offsetWidth) / 100;
          }
          if (typeof point.y === 'number' && node.offsetHeight) {
            y += (point.y * node.offsetHeight) / 100;
          }
        }
        if (dist(viewport_to_document(ox, oy), [x, y]) > 50) {
          cfi = null;
        }
      }
      return cfi;
    };

    CanonicalFragmentIdentifier.prototype.at_current = function() {
      var cfi, cury, de, delta, deltax, deltay, direction, max, maxx, maxy, minx, miny, px, py, rect, winh, winw, winx, winy, x, x_loop, y, _i, _len, _ref, _ref1, _ref2, _ref3,
        _this = this;
      _ref = window_scroll_pos(), winx = _ref[0], winy = _ref[1];
      _ref1 = [window.innerWidth, window.innerHeight], winw = _ref1[0], winh = _ref1[1];
      max = Math.max;
      winw = max(winw, 400);
      winh = max(winh, 600);
      deltay = Math.floor(winh / 50);
      deltax = Math.floor(winw / 25);
      miny = max(-winy, -winh);
      maxy = winh;
      minx = max(-winx, -winw);
      maxx = winw;
      x_loop = function(cury) {
        var cfi, curx, delta, direction, _i, _len, _ref2;
        _ref2 = [-1, 1];
        for (_i = 0, _len = _ref2.length; _i < _len; _i++) {
          direction = _ref2[_i];
          delta = deltax * direction;
          curx = 0;
          while (!((direction < 0 && curx < minx) || (direction > 0 && curx > maxx))) {
            cfi = _this.at_point(curx, cury);
            if (cfi) {
              return cfi;
            }
            curx += delta;
          }
        }
        return null;
      };
      _ref2 = [-1, 1];
      for (_i = 0, _len = _ref2.length; _i < _len; _i++) {
        direction = _ref2[_i];
        delta = deltay * direction;
        cury = 0;
        while (!((direction < 0 && cury < miny) || (direction > 0 && cury > maxy))) {
          cfi = x_loop(cury, -1);
          if (cfi) {
            return cfi;
          }
          cury += delta;
        }
      }
      _ref3 = window_scroll_pos(), x = _ref3[0], y = _ref3[1];
      de = document.documentElement;
      rect = de.getBoundingClientRect();
      px = (x * 100) / rect.width;
      py = (y * 100) / rect.height;
      cfi = "/2@" + (fstr(px)) + ":" + (fstr(py));
      return cfi;
    };

    return CanonicalFragmentIdentifier;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.cfi = new CanonicalFragmentIdentifier();
  } else if (typeof process !== "undefined" && process !== null) {
    cfi = new CanonicalFragmentIdentifier();
    t = 'a^!,1';
    log(t);
    log(escape_for_cfi(t));
    log(unescape_from_cfi(escape_for_cfi(t)));
  }

}).call(this);
