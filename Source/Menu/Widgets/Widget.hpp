#pragma once

namespace Menu {
	class Widget {
	public:
		virtual ~Widget() = default;
		virtual void draw() = 0;
	};
}