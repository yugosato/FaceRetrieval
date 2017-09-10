#pragma warning(disable: 4819)
#include "ofMain.h"
#pragma warning(default: 4819)
#include <iostream>
#include "boost/scoped_ptr.hpp"

// Uncomment to specify hooks by `std::function' (standardized in C++11)
//#define SCROLLBAR4OF_CPP11_STD_FUNCTION

#ifdef SCROLLBAR4OF_CPP11_STD_FUNCTION
#include <functional>
#endif // SCROLLBAR4OF_CPP11_STD_FUNCTION

template <class Location>
class ScrollBarBase {
public:
	class ScrollBarHook {
	public:
		virtual void action(ScrollBarBase<Location> &) = 0;
	};

	class ScrollBarHookEmpty : public ScrollBarHook {
	public:
		void action(ScrollBarBase<Location> &) { }
	};

#ifdef SCROLLBAR4OF_CPP11_STD_FUNCTION
	typedef std::function<void(ScrollBarBase<Location> &)> hook_function_type;

	class ScrollBarHookFunction : public ScrollBarHook {
	private:
		hook_function_type func_;
	public:
		ScrollBarHookFunction(hook_function_type func) : func_(func) {}
		void action(ScrollBarBase<Location> & scrollbar) { func_(scrollbar); }
	};
#endif // SCROLLBAR4OF_CPP11_STD_FUNCTION

private:
	int border_weight_;
	uint64_t duration_first_;
	uint64_t duration_step_;
	int bar_pos_widthdir_, bar_pos_lengthdir_, bar_width_, bar_length_;
	int min_, max_, current_, change_by_button_, change_by_bar_;

public:
	int scrollable_size_, knob_size_;

	// Variables for scroll bar manipulations
	int dragged_pos_; // The position (in the length direction) where the knob is dragged; -1 if not dragged

	uint64_t button_pressed_time_; // The time the button or the bar is clicked
	int button_press_count_; // Just after the click: 2, Keep pressing the mouse button: 1, No click: 0
	int button_press_move_limit_; // The limit of the value change (determined by the mouse position)
	bool minus_button_pressed_, plus_button_pressed_, bar_pressed_;

	boost::scoped_ptr<ScrollBarHook> hook_;

	// Cache the sizes for drawing the scroll bar
	void update_bar_size_info() {
		// The length of the bar (the region the knob can move; excluding the two buttons at the ends)
		scrollable_size_ = bar_length_ - 2 * bar_width_;

		// The length of the knob
		knob_size_ = scrollable_size_ * change_by_bar_ / (max_ - min_ + 1);
		if (knob_size_ < bar_width_ / 2) {
			knob_size_ = bar_width_ / 2;
		}
		if (knob_size_ > scrollable_size_ / 2) {
			if (max_ == min_) {
				knob_size_ = scrollable_size_;
			}
			else {
				knob_size_ = scrollable_size_ / 2;
			}
		}
	}

	int get_knob_top() {
		if (max_ == min_) {
			return bar_pos_lengthdir_ + bar_width_;
		}
		else {
			return bar_pos_lengthdir_ + bar_width_ + (scrollable_size_ - knob_size_) * (current_ - min_) / (max_ - min_);
		}
	}

	// If `max_' is invalid in comparison with `min_', fixes `max_'
	void check_min_max() {
		if (min_ > max_) {
			//std::cerr << "Warning: the minimum of the scroll-bar value is smaller than the maximum. Adjusted the maximum to the minimum." << std::endl;
			max_ = min_;
		}
	}

	// If `min_' is invalid in comparison with `max_', fixes `min_'
	void check_max_min() {
		if (min_ > max_) {
			//std::cerr << "Warning: the minimum of the scroll-bar value is smaller than the maximum. Adjusted the minimum to the maximum." << std::endl;
			min_ = max_;
		}
	}

	// If `current_' is invalid in comparison with `min_', fixes `current_'
	void check_min_current() {
		if (min_ > current_) {
			//std::cerr << "Warning: current scroll-bar value is smaller than the minimum. Adjusted the current to the minimum." << std::endl;
			current_ = min_;
		}
	}

	// If `current_' is invalid in comparison with `max_', fixes `current_'
	void check_max_current() {
		if (max_ < current_) {
			//std::cerr << "Warning: current scroll-bar value is larger than the maximum. Adjusted the current to the maximum." << std::endl;
			current_ = max_;
		}
	}

	// Sign
	int sgn(int val) {
		if (val < 0) return -1;
		if (val > 0) return 1;
		return 0;
	}

	// Check the specified position is in the scroll bar
	bool in_scrollbar(int clickpos_widthdir, int clickpos_lengthdir) {
		if (clickpos_widthdir < bar_pos_widthdir_ || clickpos_widthdir >= bar_pos_widthdir_ + bar_width_ ||
			clickpos_lengthdir < bar_pos_lengthdir_ || clickpos_lengthdir >= bar_pos_lengthdir_ + bar_length_) {
			return false;
		}
		return true;
	}

	// Draw a square border
	void draw_border(int pos_w, int pos_l, int size_w, int size_l) {
		for (int i = 0; i < border_weight_; ++i) {
			int endpos_w = pos_w + size_w - 1;
			int endpos_l = pos_l + size_l - 1;
			// right/bottom sides
			// Color: 0 -> 128
			ofSetColor(128 * i / (border_weight_ - 1));
			Location::draw_line(endpos_w - i, pos_l + i, endpos_w - i, endpos_l - i);
			Location::draw_line(pos_w + i, endpos_l - i, endpos_w - i, endpos_l - i);
			// top/left sides
			// Color: 224 -> 192
			ofSetColor(224 - 32 * i / (border_weight_ - 1));
			Location::draw_line(pos_w + i, pos_l + i, pos_w + i, endpos_l - i);
			Location::draw_line(pos_w + i, pos_l + i, endpos_w - i, pos_l + i);
		}
	}

	// Compute the scroll bar value from the mouse position under dragged
	int DraggedCoord2Value(int x, int y) {
		int clickpos_widthdir, clickpos_lengthdir;
		if (scrollable_size_ <= knob_size_) {
			// If the scroll bar is fixed
			return min_;
		}else{
			Location::convert_coord(clickpos_widthdir, clickpos_lengthdir, x, y);
			int base_pos = (dragged_pos_ >= 0 ? dragged_pos_ : knob_size_ / 2);
			return min_ + (max_ - min_) * (clickpos_lengthdir - (bar_pos_lengthdir_ + bar_width_ + base_pos)) / (scrollable_size_ - knob_size_);
		}
	}

public:
	ScrollBarBase()
		: border_weight_(2), duration_first_(250), duration_step_(50),
		bar_pos_widthdir_(0), bar_pos_lengthdir_(0), bar_width_(0), bar_length_(0),
		min_(0), max_(100), current_(0), change_by_button_(1), change_by_bar_(10),
		dragged_pos_(-1), button_press_count_(0),
		minus_button_pressed_(false), plus_button_pressed_(false), bar_pressed_(false),
		hook_(new ScrollBarHookEmpty())
	{}

	// The weight of the border lines.
	int border_weight() const { return border_weight_; }
	void border_weight(int border_weight_new) { border_weight_ = border_weight_new; }
	// After the first click on the scroll bar, keeping pressing a mouse button for this time (msec) restarts the knob moving.
	int duration_first() const { return duration_first_; }
	void duration_first(int duration_first_new) { duration_first_ = duration_first_new; }
	// After keeping pressing a mouse button for duration_first(), the knob moves with the interval of this time (msec).
	int duration_step() const { return duration_step_; }
	void duration_step(int duration_step_new) { duration_step_ = duration_step_new; }
	// The position of the scroll bar for the direction of the width of the scroll bar
	// (vertical scroll bar: the left end, horizontal scroll bar: the top end)
	float bar_pos_widthdir() const { return bar_pos_widthdir_; }
	void bar_pos_widthdir(float bar_pos_widthdir_new) { bar_pos_widthdir_ = bar_pos_widthdir_new; }
	// The position of the scroll bar for the direction of the length of the scroll bar
	// (vertical scroll bar: the top end, horizontal scroll bar: the left end)
	float bar_pos_lengthdir() const { return bar_pos_lengthdir_; }
	void bar_pos_lengthdir(float bar_pos_lengthdir_new) { bar_pos_lengthdir_ = bar_pos_lengthdir_new; }

	// The displayed width of the scroll bar
	// (vertical scroll bar: horizontal display size, horizontal scroll bar: vertical display size)
	float bar_width() const { return bar_width_; }
	void bar_width(float bar_width_new) {
		bar_width_ = bar_width_new;
		update_bar_size_info();
	}
	// The displayed length of the scroll bar
	// (vertical scroll bar: vertical display size, horizontal scroll bar: horizontal display size)
	float bar_length() const { return bar_length_; }
	void bar_length(float bar_length_new) {
		bar_length_ = bar_length_new;
		update_bar_size_info();
	}

	// The number of the change of the scroll bar value when one of two buttons at the end is clicked
	int change_by_button() const { return change_by_button_; }
	void change_by_button(int change_by_button_new) {
		change_by_button_ = change_by_button_new;
		if (change_by_button_ <= 0) change_by_button_ = 1;
	}

	// The number of the change of the scroll bar value when the bar itself is clicked
	int change_by_bar() const { return change_by_bar_; }
	void change_by_bar(int change_by_bar_new) {
		change_by_bar_ = change_by_bar_new;
		if (change_by_bar_ <= 0) change_by_bar_ = 1;
		update_bar_size_info();
	}

	// The minimum value of the scroll bar
	int min() const { return min_; }
	void min(int min_new) {
		if (min_ == min_new) return;

		min_ = min_new;
		check_min_max();
		check_min_current();
		update_bar_size_info();
		try {
			hook_->action(*this);
		}
		catch(std::exception e){
			std::cout << "min: Something is wrong with calling the hook! - " << e.what() << std::endl;
		}
	}

	// The maximum value of the scroll bar
	int max() const { return max_; }
	void max(int max_new) {
		if (max_ == max_new) return;

		max_ = max_new;
		check_max_min();
		check_max_current();
		update_bar_size_info();
		try {
			hook_->action(*this);
		}
		catch (std::exception e) {
			std::cout << "max: Something is wrong with calling the hook! - " << e.what() << std::endl;
		}
	}

	// Current value of the scroll bar
	int current() const { return current_; }
	void current(int current_new) {
		if (current_ == current_new) return;

		current_ = current_new;
		check_min_current();
		check_max_current();
		try {
			hook_->action(*this);
		}
		catch (std::exception e) {
			std::cout << "current: Something is wrong with calling the hook! - " << e.what() << std::endl;
		}
	}

	// Hook
	void hook(ScrollBarHook * new_hook_ptr) {
		hook_.reset(new_hook_ptr);
		try {
			hook_->action(*this);
		}
		catch (std::exception e) {
			std::cout << "hook: Something is wrong with calling the hook! - " << e.what() << std::endl;
		}
	}

#ifdef SCROLLBAR4OF_CPP11_STD_FUNCTION
	void hook(hook_function_type f) {
		hook_.reset(new ScrollBarHookFunction(f));
		try {
			hook_->action(*this);
		}
		catch (std::exception e) {
			std::cout << "hook: Something is wrong with calling the hook! - " << e.what() << std::endl;
		}
	}
#endif

	// The method should be called when the mouse is pressed on the scroll bar.
	// Call this in `mousePressed' event defined by openFrameworks.
	// If the mouse is not on the scroll bar, this returns false; otherwise true.
	bool mousePressed(int x, int y) {
		dragged_pos_ = -1;

		int clickpos_widthdir, clickpos_lengthdir;
		Location::convert_coord(clickpos_widthdir, clickpos_lengthdir, x, y);
		if(!in_scrollbar(clickpos_widthdir, clickpos_lengthdir)) return false;

		// position of the knob
		int knob_top = get_knob_top();

		// Where is clicked?
		if (clickpos_lengthdir < bar_pos_lengthdir_ + bar_width_) {
			// Button for minus
			button_press_count_ = 2;
			minus_button_pressed_ = true;
			button_pressed_time_ = ofGetElapsedTimeMillis();
			current(current_ - change_by_button_);
		} else if (clickpos_lengthdir >= bar_pos_lengthdir_ + bar_length_ - bar_width_) {
			// Button for plus
			button_press_count_ = 2;
			plus_button_pressed_ = true;
			button_pressed_time_ = ofGetElapsedTimeMillis();
			current(current_ + change_by_button_);
		} else if (clickpos_lengthdir < knob_top) {
			// Bar, left/upper side of the knob
			button_press_count_ = 2;
			bar_pressed_ = true;
			button_press_move_limit_ = DraggedCoord2Value(x, y);
			button_pressed_time_ = ofGetElapsedTimeMillis();
			current(current_ - change_by_bar_);
		} else if (clickpos_lengthdir >= knob_top + knob_size_) {
			// Bar, right/bottom side of the knob
			button_press_count_ = 2;
			bar_pressed_ = true;
			button_press_move_limit_ = DraggedCoord2Value(x, y);
			button_pressed_time_ = ofGetElapsedTimeMillis();
			current(current_ + change_by_bar_);
		} else {
			// Knob
			dragged_pos_ = clickpos_lengthdir - knob_top;
			if (dragged_pos_ < 0) dragged_pos_ = 0;
		}

		return true;
	}

	// The method should be called when the mouse is dragged after clicking the scroll bar.
	// Call this in `mouseDragged' event defined by openFrameworks.
	// If the drag is for the scroll bar, this returns true; otherwise false.
	bool mouseDragged(int x, int y) {
		if (dragged_pos_ >= 0) {
			current(DraggedCoord2Value(x, y));
			return true;
		}
		else if (button_press_count_) {
			button_press_move_limit_ = DraggedCoord2Value(x, y);
		}
		return false;
	}

	// The method should be called when the mouse is released after clicking the scroll bar.
	// Call this in `mouseReleased' event defined by openFrameworks.
	// If the release is for the scroll bar, this returns true; otherwise false.
	bool mouseReleased(int x, int y) {
		if (dragged_pos_ >= 0) {
			dragged_pos_ = -1;
			return true;
		}
		else if (button_press_count_) {
			button_press_count_ = 0;
			minus_button_pressed_ = false;
			plus_button_pressed_ = false;
			bar_pressed_ = false;
			return true;
		}
		else {
			return false;
		}
	}

	// The method should be called for time-dependent processes of the scroll bar.
	// Returns whether some operations are done.
	// Call this in `update' event defined by openFrameworks.
	bool update() {
		if (!button_press_count_) return false;

		int change;
		if (minus_button_pressed_) {
			change = -change_by_button_;
		}
		else if (plus_button_pressed_) {
			change = change_by_button_;
		}
		else if (bar_pressed_) {
			change = change_by_bar_ * sgn(button_press_move_limit_ - current_);
		}

		int newval;

		uint64_t now = ofGetElapsedTimeMillis();
		if (button_press_count_ == 2) {
			if(now - button_pressed_time_ >= duration_first_) {
				button_pressed_time_ += duration_first_;
				button_press_count_ = 1;

				newval = current_ + change;
				if (bar_pressed_ && sgn(current_ - button_press_move_limit_) * sgn(newval - button_press_move_limit_) < 0) {
					newval = button_press_move_limit_;
				}
				current(newval);
			}
			else {
				return false;
			}
		}

		while (now - button_pressed_time_ >= duration_step_) {
			button_pressed_time_ += duration_step_;
			newval = current_ + change;
			if (bar_pressed_ && sgn(current_ - button_press_move_limit_) * sgn(newval - button_press_move_limit_) < 0) {
				newval = button_press_move_limit_;
			}
			current(newval);
		}

		return true;
	}

	// The method should be called to draw the scroll bar.
	// Call this in `draw' event defined by openFrameworks.
	void draw() {
		// Right/bottom positions of the scroll bar
		int bar_endpos_widthdir_ = bar_pos_widthdir_ + bar_width_ - 1;
		int bar_endpos_lengthdir_ = bar_pos_lengthdir_ + bar_length_ - 1;

		// The position of the knob
		int knob_top = get_knob_top();
		int knob_bottom = knob_top + knob_size_ - 1;
		int i_new;

		// ---------- Filling
		// Whole the scroll bar
		ofSetColor(255);
		ofFill();
		Location::draw_rectangle(bar_pos_widthdir_, bar_pos_lengthdir_, bar_width_ - 1, bar_length_ - 1);

		// Knob
		ofSetColor(128);
		ofFill();
		i_new = border_weight_ * 2;
		Location::draw_rectangle(bar_pos_widthdir_ + i_new, knob_top + i_new, bar_width_ - 2 * i_new - 1, knob_size_ - 2 * i_new - 1);

		// Increment/decrement button
		ofSetColor(255);
		ofFill();
		Location::draw_rectangle(bar_pos_widthdir_ + border_weight_, bar_pos_lengthdir_ + border_weight_, bar_width_ - border_weight_ * 2, bar_width_ - border_weight_ * 2);
		Location::draw_rectangle(bar_pos_widthdir_ + border_weight_, bar_pos_lengthdir_ + border_weight_ + bar_length_ - bar_width_, bar_width_ - border_weight_ * 2, bar_width_ - border_weight_ * 2);

		// ---------- Borders of the whole scroll bar
		// Right/Bottom
		// Brightness: 224��192
		for (int i = 0; i < border_weight_; ++i) {
			ofSetColor(224 - 32 * i / (border_weight_ - 1));
			Location::draw_line(bar_endpos_widthdir_ - i, bar_pos_lengthdir_ + bar_width_ + i, bar_endpos_widthdir_ - i, bar_endpos_lengthdir_ - bar_width_ - i);
			Location::draw_line(bar_pos_widthdir_ + i, bar_endpos_lengthdir_ - bar_width_ - i, bar_endpos_widthdir_ - i, bar_endpos_lengthdir_ - bar_width_ - i);
		}

		// Left/Top
		// Brightness�F0��128
		for (int i = 0; i < border_weight_; ++i) {
			ofSetColor(128 * i / (border_weight_ - 1));
			Location::draw_line(bar_pos_widthdir_ + i, bar_pos_lengthdir_ + bar_width_ + i, bar_endpos_widthdir_ - i, bar_pos_lengthdir_ + bar_width_ + i);
			Location::draw_line(bar_pos_widthdir_ + i, bar_pos_lengthdir_ + bar_width_ + i, bar_pos_widthdir_ + i, bar_endpos_lengthdir_ - bar_width_ - i);
		}

		// ---------- Borders of the knob
		draw_border(bar_pos_widthdir_ + border_weight_, knob_top + border_weight_, bar_width_ - 2 * border_weight_, knob_size_ - 2 * border_weight_);

		// ---------- Borders/triangles of increment/decrement button
		draw_border(bar_pos_widthdir_, bar_pos_lengthdir_, bar_width_, bar_width_);
		ofSetColor(0);
		ofFill();
		Location::draw_triangle(
			bar_pos_widthdir_ + bar_width_ / 2, bar_pos_lengthdir_ + bar_width_ / 4,
			bar_pos_widthdir_ + bar_width_ / 4, bar_pos_lengthdir_ + bar_width_ * 3 / 4,
			bar_pos_widthdir_ + bar_width_ * 3 / 4, bar_pos_lengthdir_ + bar_width_ * 3 / 4);
		draw_border(bar_pos_widthdir_, bar_pos_lengthdir_ + bar_length_ - bar_width_, bar_width_, bar_width_);
		ofSetColor(0);
		ofFill();
		Location::draw_triangle(
			bar_pos_widthdir_ + bar_width_ / 2, bar_pos_lengthdir_ + bar_length_ - bar_width_ + bar_width_ * 3 / 4,
			bar_pos_widthdir_ + bar_width_ / 4, bar_pos_lengthdir_ + bar_length_ - bar_width_ + bar_width_ / 4,
			bar_pos_widthdir_ + bar_width_ * 3 / 4, bar_pos_lengthdir_ + bar_length_ - bar_width_ + bar_width_ / 4);
	}
};

class LocationVerticalScrollBar {
public:
	static void convert_coord(int & pos_widthdir, int & pos_lengthdir, int x, int y) {
		pos_widthdir = x;
		pos_lengthdir = y;
	}

	static void draw_line(int pos_widthdir1, int pos_lengthdir1, int pos_widthdir2, int pos_lengthdir2) {
		ofDrawLine(pos_widthdir1, pos_lengthdir1, pos_widthdir2, pos_lengthdir2);
	}

	static void draw_rectangle(int pos_widthdir, int pos_lengthdir, int size_widthdir, int size_lengthdir) {
		ofDrawRectangle(pos_widthdir, pos_lengthdir, size_widthdir, size_lengthdir);
	}

	static void draw_triangle(int pos_widthdir1, int pos_lengthdir1, int pos_widthdir2, int pos_lengthdir2, int pos_widthdir3, int pos_lengthdir3) {
		ofDrawTriangle(pos_widthdir1, pos_lengthdir1, pos_widthdir2, pos_lengthdir2, pos_widthdir3, pos_lengthdir3);
	}
};

class LocationHorizontalScrollBar {
public:
	static void convert_coord(int & pos_widthdir, int & pos_lengthdir, int x, int y) {
		pos_widthdir = y;
		pos_lengthdir = x;
	}

	static void draw_line(int pos_widthdir1, int pos_lengthdir1, int pos_widthdir2, int pos_lengthdir2) {
		ofDrawLine(pos_lengthdir1, pos_widthdir1, pos_lengthdir2, pos_widthdir2);
	}

	static void draw_rectangle(int pos_widthdir, int pos_lengthdir, int size_widthdir, int size_lengthdir) {
		ofDrawRectangle(pos_lengthdir, pos_widthdir, size_lengthdir, size_widthdir);
	}

	static void draw_triangle(int pos_widthdir1, int pos_lengthdir1, int pos_widthdir2, int pos_lengthdir2, int pos_widthdir3, int pos_lengthdir3) {
		ofDrawTriangle(pos_lengthdir1, pos_widthdir1, pos_lengthdir2, pos_widthdir2, pos_lengthdir3, pos_widthdir3);
	}
};

typedef ScrollBarBase<LocationVerticalScrollBar> VerticalScrollBar;
typedef ScrollBarBase<LocationHorizontalScrollBar> HorizontalScrollBar;
