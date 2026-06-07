#include "Room.hpp"

Room::Room(std::string roomId, int capacity, RoomType type) : m_roomId(roomId), m_capacity(capacity), m_type(type) {}

std::string Room::getRoomId() const 
{ 
    return m_roomId; 
}

int Room::getCapacity() const 
{ 
    return m_capacity;

}
RoomType Room::getType() const 
{ 
    return m_type; 
}

std::string Room::getTypeAsString() const {
    switch (m_type) {
        case RoomType::Theory:      return "Theory";
        case RoomType::Lab:         return "Lab";
        case RoomType::Auditorium:  return "Auditorium";
        default:                    return "Unknown";
    }
}