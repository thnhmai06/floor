#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>	
#include <unordered_set>
#include "floor/audio/type.h"

namespace Floor::Audio::Memory
{
	struct MusicMemory
	{
		using Container = std::unordered_map<std::string, Music>;
		struct Item
		{
			const MusicMemory* parent = nullptr;
			Container::const_iterator item{};

			void reset();
			[[nodiscard]] bool is_valid() const;

			Item() = default;
			Item(const MusicMemory* parent, Container::const_iterator item);
		};

		Container items;

		[[nodiscard]] Item find(const std::string& name) const;
		Item load(const std::filesystem::path& file_path, const std::string& name, bool override = true);
		void free(const std::string& name);
		void free(const Item& item);
		void free_all();

		~MusicMemory();
	};

	struct EffectMemory
	{
		using Container = std::unordered_map<std::string, Effect>;
		struct Item
		{
			const EffectMemory* parent = nullptr;
			Container::const_iterator item{};

			void reset();
			[[nodiscard]] bool is_valid() const;

			Item() = default;
			Item(const EffectMemory* parent, Container::const_iterator item);
		};
		static std::unordered_set<std::string_view> SUPPORT_EXTENSIONS;

		Container items;

		[[nodiscard]] Item find(const std::string& name) const;
		Item load(const std::filesystem::path& file_path, const std::string& name, bool override = true);
		void load(
			const std::filesystem::path& folder_path,
			const std::filesystem::path& root,
			bool no_except = true, bool recursive = true,
			bool name_has_extension = false, bool relative_path_name = false,
			bool override = true,
			const std::unordered_set<std::filesystem::path>& blacklist = {});
		void free(const std::string& name);
		void free(Item& item);
		void free_all();

		~EffectMemory();
	};
}