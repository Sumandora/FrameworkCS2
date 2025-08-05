#pragma once

#include "Utils/UninitializedObject.hpp"
#include <string>
#include <type_traits>

namespace Libraries {
	class Library {
		std::string library_name;
		void* handle;

		[[nodiscard]] void* get_symbol(const std::string& name) const;

	public:
		explicit Library(const std::string& name);
		~Library();

		template <typename T>
			requires std::is_pointer_v<T>
		T get_symbol(const std::string& name) const
		{
			return reinterpret_cast<T>(get_symbol(name));
		}

		[[nodiscard]] const std::string& get_library_name() const { return library_name; }
	};

	inline UninitializedObject<Library> client;
	inline UninitializedObject<Library> schemasystem;
	inline UninitializedObject<Library> tier0;
	inline UninitializedObject<Library> engine2;
	inline UninitializedObject<Library> panorama;
	inline UninitializedObject<Library> materialsystem2;
	inline UninitializedObject<Library> resourcesystem;
	inline UninitializedObject<Library> filesystem;
	inline UninitializedObject<Library> localize;
	inline UninitializedObject<Library> particles;

	void create();
	void destroy();
}
