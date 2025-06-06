#include "floor/audio/memory.h" // Header
#include <SDL3_mixer/SDL_mixer.h>
#include <ranges>
#include "floor/exceptions.h"

namespace Floor::Audio::Memory
{
	//! Memory<Music>
	Item<Music> Memory<Music>::find(const std::string& name) const
	{
		if (const auto& itr = items.find(name); itr != items.end())
			return { this, itr };
		return { nullptr, items.end() };
	}
	void Memory<Music>::free(const std::string& name)
	{
		if (const auto itr = items.find(name); itr != items.end())
		{
			Mix_FreeMusic(itr->second);
			items.erase(itr);
		}
	}
	void Memory<Music>::free(const Item<Music>& item)
	{
		if (item.is_valid() && item.parent == this)
			free(item.itr->first);
	}
	void Memory<Music>::free()
	{
		for (const auto& music : items | std::views::values)
			Mix_FreeMusic(music);
		items.clear();
	}
	Item<Music> Memory<Music>::load(const std::filesystem::path& file_path, const std::string& name, const bool override)
	{
		if (!override && items.contains(name))
			return {};

		auto audio = Mix_LoadMUS(file_path.string().c_str());
		if (!audio) throw Exceptions::SDL_Exception();

		return { this, items.insert_or_assign(name, audio).first };
	}
	Memory<Music>::~Memory() { free(); }

	//! Memory<Effect>
	Item<Effect> Memory<Effect>::find(const std::string& name) const
	{
		if (const auto& itr = items.find(name); itr != items.end())
			return { this, itr };

		return { nullptr, items.end() };
	}
	void Memory<Effect>::free(const std::string& name)
	{
		if (const auto itr = items.find(name); itr != items.end())
		{
			Mix_FreeChunk(itr->second);
			items.erase(itr);
		}
	}
	void Memory<Effect>::free(Item<Effect>& item)
	{
		if (item.is_valid() && item.parent == this)
		{
			free(item.itr->first);
			item.reset();
		}
	}
	void Memory<Effect>::free()
	{
		for (const auto& effect : items | std::views::values)
			Mix_FreeChunk(effect);
		items.clear();
	}
	Item<Effect> Memory<Effect>::load(const std::filesystem::path& file_path, const std::string& name, const bool override)
	{
		if (!override && items.contains(name))
			return {};

		auto audio = Mix_LoadWAV(file_path.string().c_str());
		if (!audio) throw Exceptions::SDL_Exception();

		return { this, items.insert_or_assign(name, audio).first };
	}
	void Memory<Effect>::load(
		const std::filesystem::path& folder_path,
		const std::filesystem::path& root_folder_path,
		const bool no_except, const bool recursive,
		const bool name_has_extension, const bool name_is_filename, const bool override,
		const std::unordered_set<std::string>& only_extensions,
		const std::unordered_set<std::filesystem::path>& blacklist)
	{
		if (std::filesystem::exists(folder_path) && std::filesystem::is_directory(folder_path))
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path))
			{
				if (recursive && std::filesystem::is_directory(entry))
					load(entry, root_folder_path, no_except, recursive,
						name_has_extension, name_is_filename, override, only_extensions, blacklist);
				else if (std::filesystem::is_regular_file(entry))
				{
					// Make name
					auto name_path = entry.path().lexically_relative(root_folder_path);
					if (const auto extension = entry.path().extension().string();
						!only_extensions.empty() && !only_extensions.contains(extension))
						continue;
					if (name_is_filename)
						name_path = name_path.filename();
					if (!name_has_extension)
						name_path.replace_extension();
					const auto name = name_path.generic_string();

					// Add item
					if (blacklist.contains(name)) continue;
					if (!override && items.contains(name)) continue;
					try
					{
						load(entry, name);
					}
					catch (const Exceptions::SDL_Exception&)
					{
						if (!no_except) throw;
					}
				}
			}
		}
	}
	Memory<Effect>::~Memory() { free(); }
}
