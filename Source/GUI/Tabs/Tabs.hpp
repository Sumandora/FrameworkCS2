#pragma once

namespace GUI::Tabs {
	namespace Debug {
		void draw_interfaces();
		void draw_link_maps();
		void draw_view_matrix();
		void draw_entity_list();
		void draw_convars();
		void draw_local_player();
		void draw_event_list();
		void draw_panorama();
		void draw_notifications();
		void draw_force_update();
	}
	namespace About {
		void draw_configs();
		void draw_licenses();
	}
	void render();
}
