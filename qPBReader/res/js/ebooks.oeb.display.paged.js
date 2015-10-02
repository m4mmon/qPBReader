
/*
 Copyright 2012, Kovid Goyal <kovid@kovidgoyal.net>
 Released under the GPLv3 License
*/


(function() {
  var PagedDisplay, create_page_div, first_child, has_start_text, log, runscripts;

  log = window.calibre_utils.log;

  runscripts = function(parent) {
    var script, _i, _len, _ref, _results;
    _ref = parent.getElementsByTagName('script');
    _results = [];
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      script = _ref[_i];
      _results.push(eval(script.text || script.textContent || script.innerHTML || ''));
    }
    return _results;
  };

  first_child = function(parent) {
    var c, count;
    c = parent.firstChild;
    count = 0;
    while ((c != null ? c.nodeType : void 0) !== 1 && count < 20) {
      c = c != null ? c.nextSibling : void 0;
      count += 1;
    }
    if ((c != null ? c.nodeType : void 0) === 1) {
      return c;
    }
    return null;
  };

  has_start_text = function(elem) {
    var c, _i, _len, _ref, _ref1;
    _ref = elem.childNodes;
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      c = _ref[_i];
      if ((_ref1 = c.nodeType) !== Node.TEXT_NODE && _ref1 !== Node.COMMENT_NODE && _ref1 !== Node.PROCESSING_INSTRUCTION_NODE) {
        break;
      }
      if (c.nodeType === Node.TEXT_NODE && c.nodeValue !== null && /\S/.test(c.nodeValue)) {
        return true;
      }
    }
    return false;
  };

  create_page_div = function(elem) {
    var div;
    div = document.createElement('blank-page-div');
    div.innerText = ' \n    ';
    document.body.appendChild(div);
    div.style.setProperty('-webkit-column-break-before', 'always');
    div.style.setProperty('display', 'block');
    div.style.setProperty('white-space', 'pre');
    div.style.setProperty('background-color', 'transparent');
    div.style.setProperty('background-image', 'none');
    div.style.setProperty('border-width', '0');
    div.style.setProperty('float', 'none');
    return div.style.setProperty('position', 'static');
  };

  PagedDisplay = (function() {

    PagedDisplay.name = 'PagedDisplay';

    function PagedDisplay() {
      if (!this instanceof arguments.callee) {
        throw new Error('PagedDisplay constructor called as function');
      }
      this.set_geometry();
      this.page_width = 0;
      this.screen_width = 0;
      this.side_margin = 0;
      this.in_paged_mode = false;
      this.current_margin_side = 0;
      this.is_full_screen_layout = false;
      this.max_col_width = -1;
      this.max_col_height = -1;
      this.current_page_height = null;
      this.document_margins = null;
      this.use_document_margins = false;
      this.footer_template = null;
      this.header_template = null;
      this.header = null;
      this.footer = null;
      this.hf_style = null;
    }

    PagedDisplay.prototype.read_document_margins = function() {
      var prop, pxval, rule, sheet, tmp, val, _i, _j, _k, _len, _len1, _len2, _ref, _ref1, _ref2;
      if (this.document_margins === null) {
        this.document_margins = {
          left: null,
          top: null,
          right: null,
          bottom: null
        };
        tmp = document.createElement('div');
        tmp.style.visibility = 'hidden';
        tmp.style.position = 'absolute';
        document.body.appendChild(tmp);
        _ref = document.styleSheets;
        for (_i = 0, _len = _ref.length; _i < _len; _i++) {
          sheet = _ref[_i];
          _ref1 = sheet.rules;
          for (_j = 0, _len1 = _ref1.length; _j < _len1; _j++) {
            rule = _ref1[_j];
            if (rule.type === CSSRule.PAGE_RULE) {
              _ref2 = ['left', 'top', 'bottom', 'right'];
              for (_k = 0, _len2 = _ref2.length; _k < _len2; _k++) {
                prop = _ref2[_k];
                val = rule.style.getPropertyValue('margin-' + prop);
                if (val) {
                  tmp.style.height = val;
                  pxval = parseInt(window.getComputedStyle(tmp).height);
                  if (!isNaN(pxval)) {
                    this.document_margins[prop] = pxval;
                  }
                }
              }
            }
          }
        }
        document.body.removeChild(tmp);
        if (this.document_margins.left === null) {
          val = parseInt(window.getComputedStyle(document.body).marginLeft);
          if (!isNaN(val)) {
            this.document_margins.left = val;
          }
        }
        if (this.document_margins.right === null) {
          val = parseInt(window.getComputedStyle(document.body).marginRight);
          if (!isNaN(val)) {
            return this.document_margins.right = val;
          }
        }
      }
    };

    PagedDisplay.prototype.set_geometry = function(cols_per_screen, margin_top, margin_side, margin_bottom) {
      if (cols_per_screen == null) {
        cols_per_screen = 1;
      }
      if (margin_top == null) {
        margin_top = 20;
      }
      if (margin_side == null) {
        margin_side = 40;
      }
      if (margin_bottom == null) {
        margin_bottom = 20;
      }
      this.cols_per_screen = cols_per_screen;
      if (this.use_document_margins && this.document_margins !== null) {
        this.margin_top = this.document_margins.top || margin_top;
        this.margin_bottom = this.document_margins.bottom || margin_bottom;
        this.margin_side = this.document_margins.left || this.document_margins.right || margin_side;
        this.effective_margin_top = this.margin_top;
        return this.effective_margin_bottom = this.margin_bottom;
      } else {
        this.margin_top = margin_top;
        this.margin_side = margin_side;
        this.margin_bottom = margin_bottom;
        this.effective_margin_top = this.margin_top;
        return this.effective_margin_bottom = this.margin_bottom;
      }
    };

    PagedDisplay.prototype.handle_rtl_body = function(body_style) {
      var node, _i, _len, _ref;
      if (body_style.direction === "rtl") {
        _ref = document.body.childNodes;
        for (_i = 0, _len = _ref.length; _i < _len; _i++) {
          node = _ref[_i];
          if (node.nodeType === node.ELEMENT_NODE && window.getComputedStyle(node).direction === "rtl") {
            node.style.setProperty("direction", "rtl");
          }
        }
        document.body.style.direction = "ltr";
        return document.documentElement.style.direction = 'ltr';
      }
    };

    PagedDisplay.prototype.layout = function(is_single_page) {
      var adjust, body_style, body_width, bs, c, c2, col_width, cprop, edge, eh, fgcolor, first_layout, has_svg, n, ncols, num, only_img, priority, prop, rule, sheet, single_screen, sm, val, ww, _i, _j, _k, _l, _len, _len1, _len2, _len3, _ref, _ref1, _ref2, _ref3, _ref4;
      if (is_single_page == null) {
        is_single_page = false;
      }
      body_style = window.getComputedStyle(document.body);
      bs = document.body.style;
      first_layout = false;
      if (!this.in_paged_mode) {
        single_screen = document.body.scrollHeight < window.innerHeight + 75;
        this.handle_rtl_body(body_style);
        first_layout = true;
        if (!single_screen && this.cols_per_screen > 1) {
          num = this.cols_per_screen - 1;
          while (num > 0) {
            num -= 1;
            create_page_div();
          }
        }
      }
      ww = window.innerWidth;
      n = this.cols_per_screen;
      adjust = ww - Math.floor(ww / n) * n;
      sm = Math.max(2 * adjust, this.margin_side);
      col_width = Math.max(100, ((ww - adjust) / n) - 2 * sm);
      if (this.max_col_width > 0 && col_width > this.max_col_width) {
        sm += Math.ceil((col_width - this.max_col_width) / 2 * n);
        col_width = Math.max(100, ((ww - adjust) / n) - 2 * sm);
      }
      this.col_width = col_width;
      this.page_width = col_width + 2 * sm;
      this.side_margin = sm;
      this.screen_width = this.page_width * this.cols_per_screen;
      fgcolor = body_style.getPropertyValue('color');
      bs.setProperty('-webkit-column-gap', 2 * sm + 'px');
      bs.setProperty('-webkit-column-width', col_width + 'px');
      bs.setProperty('-webkit-column-rule', '0px inset blue');
      bs.setProperty('-webkit-margin-collapse', 'separate');
      c = first_child(document.body);
      if (c !== null) {
        c.style.setProperty('-webkit-margin-before', '0');
        c.style.setProperty('-webkit-column-break-before', 'avoid');
        if (((_ref = c.tagName) != null ? _ref.toLowerCase() : void 0) === 'div') {
          c2 = first_child(c);
          if (c2 !== null && !has_start_text(c)) {
            c2.style.setProperty('-webkit-column-break-before', 'avoid');
          }
        }
      }
      this.effective_margin_top = this.margin_top;
      this.effective_margin_bottom = this.margin_bottom;
      this.current_page_height = window.innerHeight - this.margin_top - this.margin_bottom;
      if (this.max_col_height > 0 && this.current_page_height > this.max_col_height) {
        eh = Math.ceil((this.current_page_height - this.max_col_height) / 2);
        this.effective_margin_top += eh;
        this.effective_margin_bottom += eh;
        this.current_page_height -= 2 * eh;
      }
      bs.setProperty('overflow', 'visible');
      bs.setProperty('height', this.current_page_height + 'px');
      bs.setProperty('width', (window.innerWidth - 2 * sm) + 'px');
      bs.setProperty('margin-top', this.effective_margin_top + 'px');
      bs.setProperty('margin-bottom', this.effective_margin_bottom + 'px');
      bs.setProperty('margin-left', sm + 'px');
      bs.setProperty('margin-right', sm + 'px');
      _ref1 = ['left', 'right', 'top', 'bottom'];
      for (_i = 0, _len = _ref1.length; _i < _len; _i++) {
        edge = _ref1[_i];
        bs.setProperty('padding-' + edge, '0px');
        bs.setProperty('border-' + edge + '-width', '0px');
      }
      bs.setProperty('min-width', '0');
      bs.setProperty('max-width', 'none');
      bs.setProperty('min-height', '0');
      bs.setProperty('max-height', 'none');
      _ref2 = document.styleSheets;
      for (_j = 0, _len1 = _ref2.length; _j < _len1; _j++) {
        sheet = _ref2[_j];
        _ref3 = sheet.rules;
        for (_k = 0, _len2 = _ref3.length; _k < _len2; _k++) {
          rule = _ref3[_k];
          if (rule.type === CSSRule.STYLE_RULE) {
            _ref4 = ['page-break-before', 'page-break-after', 'page-break-inside'];
            for (_l = 0, _len3 = _ref4.length; _l < _len3; _l++) {
              prop = _ref4[_l];
              val = rule.style.getPropertyValue(prop);
              if (val) {
                cprop = '-webkit-column-' + prop.substr(5);
                priority = rule.style.getPropertyPriority(prop);
                rule.style.setProperty(cprop, val, priority);
              }
            }
          }
        }
      }
      if (first_layout) {
        has_svg = document.getElementsByTagName('svg').length > 0;
        only_img = document.getElementsByTagName('img').length === 1 && document.getElementsByTagName('div').length < 3 && document.getElementsByTagName('p').length < 2;
        body_width = document.body.offsetWidth + 2 * sm;
        this.is_full_screen_layout = (only_img || has_svg) && single_screen && document.body.scrollWidth > body_width && document.body.scrollWidth < 2 * body_width;
        if (is_single_page) {
          this.is_full_screen_layout = true;
        }
      }
      ncols = document.body.scrollWidth / this.page_width;
      if (ncols !== Math.floor(ncols) && !this.is_full_screen_layout) {
        bs.setProperty('width', Math.floor(ncols) * this.page_width - 2 * sm);
      }
      this.in_paged_mode = true;
      this.current_margin_side = sm;
      return sm;
    };

    PagedDisplay.prototype.create_header_footer = function(uuid) {
      if (this.header_template !== null) {
        this.header = document.createElement('div');
        this.header.setAttribute('style', "overflow:hidden; display:block; position:absolute; left:" + this.side_margin + "px; top: 0px; height: " + this.effective_margin_top + "px; width: " + this.col_width + "px; margin: 0; padding: 0");
        this.header.setAttribute('id', 'pdf_page_header_' + uuid);
        document.body.appendChild(this.header);
      }
      if (this.footer_template !== null) {
        this.footer = document.createElement('div');
        this.footer.setAttribute('style', "overflow:hidden; display:block; position:absolute; left:" + this.side_margin + "px; top: " + (window.innerHeight - this.effective_margin_bottom) + "px; height: " + this.effective_margin_bottom + "px; width: " + this.col_width + "px; margin: 0; padding: 0");
        this.footer.setAttribute('id', 'pdf_page_footer_' + uuid);
        document.body.appendChild(this.footer);
      }
      if (this.header !== null || this.footer !== null) {
        this.hf_uuid = uuid;
        this.hf_style = document.createElement('style');
        this.hf_style.setAttribute('type', 'text/css');
        document.head.appendChild(this.hf_style);
      }
      return this.update_header_footer(1);
    };

    PagedDisplay.prototype.position_header_footer = function() {
      var left, top, _ref;
      _ref = calibre_utils.viewport_to_document(0, 0, document.body.ownerDocument), left = _ref[0], top = _ref[1];
      if (this.header !== null) {
        this.header.style.setProperty('left', left + 'px');
      }
      if (this.footer !== null) {
        return this.footer.style.setProperty('left', left + 'px');
      }
    };

    PagedDisplay.prototype.update_header_footer = function(pagenum) {
      var author, cls, section, title;
      if (this.hf_style !== null) {
        if (pagenum % 2 === 1) {
          cls = "even_page";
        } else {
          cls = "odd_page";
        }
        this.hf_style.innerHTML = "#pdf_page_header_" + this.hf_uuid + " ." + cls + ", #pdf_page_footer_" + this.hf_uuid + " ." + cls + " { display: none }";
        title = py_bridge.title();
        author = py_bridge.author();
        section = py_bridge.section();
      }
      if (this.header !== null) {
        this.header.innerHTML = this.header_template.replace(/_PAGENUM_/g, pagenum + "").replace(/_TITLE_/g, title + "").replace(/_AUTHOR_/g, author + "").replace(/_SECTION_/g, section + "");
        runscripts(this.header);
      }
      if (this.footer !== null) {
        this.footer.innerHTML = this.footer_template.replace(/_PAGENUM_/g, pagenum + "").replace(/_TITLE_/g, title + "").replace(/_AUTHOR_/g, author + "").replace(/_SECTION_/g, section + "");
        return runscripts(this.footer);
      }
    };

    PagedDisplay.prototype.fit_images = function() {
      var br, col, col_width, data, doc, images, img, left, max_width, maxh, previously_limited, rleft, rright, vimages, width, _i, _j, _k, _len, _len1, _len2, _ref, _ref1, _results;
      images = [];
      vimages = [];
      maxh = this.current_page_height;
      _ref = document.getElementsByTagName('img');
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        img = _ref[_i];
        previously_limited = calibre_utils.retrieve(img, 'width-limited', false);
        data = calibre_utils.retrieve(img, 'img-data', null);
        br = img.getBoundingClientRect();
        if (data === null) {
          data = {
            'left': br.left,
            'right': br.right,
            'height': br.height,
            'display': img.style.display
          };
          calibre_utils.store(img, 'img-data', data);
        }
        left = calibre_utils.viewport_to_document(br.left, 0, doc = img.ownerDocument)[0];
        col = this.column_at(left) * this.page_width;
        rleft = left - col - this.current_margin_side;
        width = br.right - br.left;
        rright = rleft + width;
        col_width = this.page_width - 2 * this.current_margin_side;
        if (previously_limited || rright > col_width) {
          images.push([img, col_width - rleft]);
        }
        previously_limited = calibre_utils.retrieve(img, 'height-limited', false);
        if (previously_limited || br.height > maxh) {
          vimages.push(img);
        }
        if (previously_limited) {
          img.style.setProperty('-webkit-column-break-before', 'auto');
          img.style.setProperty('display', data.display);
        }
        img.style.setProperty('-webkit-column-break-inside', 'avoid');
      }
      for (_j = 0, _len1 = images.length; _j < _len1; _j++) {
        _ref1 = images[_j], img = _ref1[0], max_width = _ref1[1];
        img.style.setProperty('max-width', max_width + 'px');
        calibre_utils.store(img, 'width-limited', true);
      }
      _results = [];
      for (_k = 0, _len2 = vimages.length; _k < _len2; _k++) {
        img = vimages[_k];
        data = calibre_utils.retrieve(img, 'img-data', null);
        img.style.setProperty('-webkit-column-break-before', 'always');
        img.style.setProperty('max-height', maxh + 'px');
        if (data.height > maxh) {
          img.style.setProperty('display', 'block');
        }
        _results.push(calibre_utils.store(img, 'height-limited', true));
      }
      return _results;
    };

    PagedDisplay.prototype.scroll_to_pos = function(frac) {
      var xpos;
      xpos = Math.floor(document.body.scrollWidth * frac);
      return this.scroll_to_xpos(xpos);
    };

    PagedDisplay.prototype.scroll_to_xpos = function(xpos, animated, notify, duration) {
      var limit, pos;
      if (animated == null) {
        animated = false;
      }
      if (notify == null) {
        notify = false;
      }
      if (duration == null) {
        duration = 1000;
      }
      if (typeof xpos !== 'number') {
        log(xpos, 'is not a number, cannot scroll to it!');
        return;
      }
      if (this.is_full_screen_layout) {
        window.scrollTo(0, 0);
        return;
      }
      pos = Math.floor(xpos / this.page_width) * this.page_width;
      limit = document.body.scrollWidth - this.screen_width;
      if (pos > limit) {
        pos = limit;
      }
      if (animated) {
        return this.animated_scroll(pos, duration = 1000, notify = notify);
      } else {
        return window.scrollTo(pos, 0);
      }
    };

    PagedDisplay.prototype.scroll_to_column = function(number) {
      return this.scroll_to_xpos(number * this.page_width + 10);
    };

    PagedDisplay.prototype.column_at = function(xpos) {
      return Math.floor(xpos / this.page_width);
    };

    PagedDisplay.prototype.column_location = function(elem) {
      var br, c, height, left, top, width, _ref, _ref1;
      br = elem.getBoundingClientRect();
      if (br.top < -100) {
        elem.scrollIntoView();
        _ref = calibre_utils.viewport_to_document(elem.scrollLeft, elem.scrollTop, elem.ownerDocument), left = _ref[0], top = _ref[1];
      } else {
        _ref1 = calibre_utils.viewport_to_document(br.left, br.top, elem.ownerDocument), left = _ref1[0], top = _ref1[1];
      }
      c = this.column_at(left);
      width = Math.min(br.right, (c + 1) * this.page_width) - br.left;
      if (br.bottom < br.top) {
        br.bottom = window.innerHeight;
      }
      height = Math.min(br.bottom, window.innerHeight) - br.top;
      left -= c * this.page_width;
      return {
        'column': c,
        'left': left,
        'top': top,
        'width': width,
        'height': height
      };
    };

    PagedDisplay.prototype.column_boundaries = function() {
      var l;
      l = this.column_at(window.pageXOffset + 10);
      return [l, l + this.cols_per_screen];
    };

    PagedDisplay.prototype.animated_scroll = function(pos, duration, notify) {
      var delta, interval, step_size, steps,
        _this = this;
      if (duration == null) {
        duration = 1000;
      }
      if (notify == null) {
        notify = true;
      }
      delta = pos - window.pageXOffset;
      interval = 50;
      steps = Math.floor(duration / interval);
      step_size = Math.floor(delta / steps);
      this.current_scroll_animation = {
        target: pos,
        step_size: step_size,
        interval: interval,
        notify: notify,
        fn: function() {
          var a, completed, npos;
          a = _this.current_scroll_animation;
          npos = window.pageXOffset + a.step_size;
          completed = false;
          if (Math.abs(npos - a.target) < Math.abs(a.step_size)) {
            completed = true;
            npos = a.target;
          }
          window.scrollTo(npos, 0);
          if (completed) {
            if (notify) {
              return window.py_bridge.animated_scroll_done();
            }
          } else {
            return setTimeout(a.fn, a.interval);
          }
        }
      };
      return this.current_scroll_animation.fn();
    };

    PagedDisplay.prototype.current_pos = function(frac) {
      var limit;
      limit = document.body.scrollWidth - window.innerWidth;
      if (limit <= 0) {
        return 0.0;
      }
      return window.pageXOffset / limit;
    };

    PagedDisplay.prototype.current_column_location = function() {
      var x;
      if (this.is_full_screen_layout) {
        return 0;
      }
      x = window.pageXOffset + Math.max(10, this.current_margin_side);
      return Math.floor(x / this.page_width) * this.page_width;
    };

    PagedDisplay.prototype.next_screen_location = function() {
      var ans, cc, limit, pages_left, width_left;
      if (this.is_full_screen_layout) {
        return -1;
      }
      cc = this.current_column_location();
      ans = cc + this.screen_width;
      if (this.cols_per_screen > 1) {
        width_left = document.body.scrollWidth - (window.pageXOffset + window.innerWidth);
        pages_left = width_left / this.page_width;
        if (Math.ceil(pages_left) < this.cols_per_screen) {
          return -1;
        }
      }
      limit = document.body.scrollWidth - window.innerWidth;
      if (ans > limit) {
        ans = window.pageXOffset < limit ? limit : -1;
      }
      return ans;
    };

    PagedDisplay.prototype.previous_screen_location = function() {
      var ans, cc;
      if (this.is_full_screen_layout) {
        return -1;
      }
      cc = this.current_column_location();
      ans = cc - this.screen_width;
      if (ans < 0) {
        ans = window.pageXOffset > 15 ? 0 : -1;
      }
      return ans;
    };

    PagedDisplay.prototype.next_col_location = function() {
      var ans, cc, limit;
      if (this.is_full_screen_layout) {
        return -1;
      }
      cc = this.current_column_location();
      ans = cc + this.page_width;
      limit = document.body.scrollWidth - window.innerWidth;
      if (ans > limit) {
        ans = window.pageXOffset < limit ? limit : -1;
      }
      return ans;
    };

    PagedDisplay.prototype.previous_col_location = function() {
      var ans, cc;
      if (this.is_full_screen_layout) {
        return -1;
      }
      cc = this.current_column_location();
      ans = cc - this.page_width;
      if (ans < 0) {
        ans = window.pageXOffset > 0 ? 0 : -1;
      }
      return ans;
    };

    PagedDisplay.prototype.jump_to_anchor = function(name) {
      var br, elem, elems, left, parent, _ref, _ref1, _ref2;
      elem = document.getElementById(name);
      if (!elem) {
        elems = document.getElementsByName(name);
        if (elems) {
          elem = elems[0];
        }
      }
      if (!elem) {
        return;
      }
      if ((_ref = window.mathjax) != null ? _ref.math_present : void 0) {
        parent = elem;
        while (parent && (parent != null ? (_ref1 = parent.tagName) != null ? _ref1.toLowerCase() : void 0 : void 0) !== 'svg') {
          parent = parent.parentNode;
        }
        if ((parent != null ? (_ref2 = parent.tagName) != null ? _ref2.toLowerCase() : void 0 : void 0) === 'svg') {
          elem = parent.parentNode;
        }
      }
      elem.scrollIntoView();
      if (this.in_paged_mode) {
        br = elem.getBoundingClientRect();
        if (br.top < -100) {
          left = elem.scrollLeft;
        } else {
          left = br.left;
        }
        return this.scroll_to_xpos(calibre_utils.viewport_to_document(left + this.margin_side, elem.scrollTop, elem.ownerDocument)[0]);
      }
    };

    PagedDisplay.prototype.snap_to_selection = function() {
      var doc, left, node, r, sel;
      if (this.in_paged_mode) {
        sel = window.getSelection();
        r = sel.getRangeAt(0).getBoundingClientRect();
        node = sel.anchorNode;
        left = calibre_utils.viewport_to_document(r.left, r.top, doc = node.ownerDocument)[0];
        return this.scroll_to_xpos(left + 5);
      }
    };

    PagedDisplay.prototype.jump_to_cfi = function(cfi, job_id) {
      var _this = this;
      if (job_id == null) {
        job_id = -1;
      }
      return window.cfi.scroll_to(cfi, function(x, y) {
        if (_this.in_paged_mode) {
          _this.scroll_to_xpos(x);
        } else {
          window.scrollTo(0, y);
        }
        if (window.py_bridge) {
          return window.py_bridge.jump_to_cfi_finished(job_id);
        }
      });
    };

    PagedDisplay.prototype.current_cfi = function() {
      var ans, c, cfi, curx, cury, deltax, deltay, left, right, x, _i, _len, _ref, _ref1, _ref2, _ref3;
      ans = null;
      if (!(window.cfi != null) || (((_ref = window.mathjax) != null ? _ref.math_present : void 0) && !((_ref1 = window.mathjax) != null ? _ref1.math_loaded : void 0))) {
        return ans;
      }
      if (this.in_paged_mode) {
        c = this.current_column_location();
        _ref2 = [c, c - this.page_width, c + this.page_width];
        for (_i = 0, _len = _ref2.length; _i < _len; _i++) {
          x = _ref2[_i];
          _ref3 = [x, x + this.page_width], left = _ref3[0], right = _ref3[1];
          if (left < 0 || right > document.body.scrollWidth) {
            continue;
          }
          deltax = Math.floor(this.page_width / 25);
          deltay = Math.floor(window.innerHeight / 25);
          cury = this.effective_margin_top;
          while (!(cury >= (window.innerHeight - this.effective_margin_bottom))) {
            curx = left + this.current_margin_side;
            while (!(curx >= (right - this.current_margin_side))) {
              cfi = window.cfi.at_point(curx - window.pageXOffset, cury - window.pageYOffset);
              if (cfi) {
                log('Viewport cfi:', cfi);
                return cfi;
              }
              curx += deltax;
            }
            cury += deltay;
          }
        }
      } else {
        try {
          ans = window.cfi.at_current();
          if (!ans) {
            ans = null;
          }
        } catch (err) {
          log(err);
        }
      }
      if (ans) {
        log('Viewport cfi:', ans);
      }
      return ans;
    };

    PagedDisplay.prototype.click_for_page_turn = function(event) {
      var left_boundary, right_bondary;
      left_boundary = this.current_margin_side;
      right_bondary = this.screen_width - this.current_margin_side;
      if (left_boundary > event.clientX) {
        return true;
      }
      if (right_bondary < event.clientX) {
        return false;
      }
      return null;
    };

    return PagedDisplay;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.paged_display = new PagedDisplay();
  }

}).call(this);
