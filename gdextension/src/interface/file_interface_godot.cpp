#include "file_interface_godot.h"
#include "../rml_util.h"

using namespace godot;

Rml::FileHandle FileInterfaceGodot::Open(const Rml::String& path) {
	Ref<FileAccess> file = FileAccess::open(rml_to_godot_string(path), FileAccess::READ);
	if (file == nullptr) {
		return 0;
	}
	 
	FileHandle *file_data = memnew(FileHandle());
    file_data->file = file;

    return reinterpret_cast<uintptr_t>(file_data);
}

void FileInterfaceGodot::Close(Rml::FileHandle file) {
	FileHandle *file_data = reinterpret_cast<FileHandle *>(file);
	file_data->file->close();
	file_data->file.unref();
}

size_t FileInterfaceGodot::Read(void* buffer, size_t size, Rml::FileHandle file) {
	FileHandle *file_data = reinterpret_cast<FileHandle *>(file);
	uint8_t *buf = static_cast<uint8_t*>(buffer);
	return file_data->file->get_buffer(buf, size);
}

bool FileInterfaceGodot::Seek(Rml::FileHandle file, long offset, int origin) {
	FileHandle *file_data = reinterpret_cast<FileHandle *>(file);
	switch (origin) {
		case SEEK_SET: {
			file_data->file->seek(offset);
		} break;
		case SEEK_CUR: {
			file_data->file->seek(file_data->file->get_position() + offset);
		} break;
		case SEEK_END: {
			file_data->file->seek_end(offset);
		} break;
	}
	return true;
}

size_t FileInterfaceGodot::Tell(Rml::FileHandle file) {
	FileHandle *file_data = reinterpret_cast<FileHandle *>(file);
	return file_data->file->get_position();
}