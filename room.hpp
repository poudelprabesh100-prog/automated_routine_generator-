#ifndef ROOM_HPP
#define ROOM_HPP

#include <string>

enum class RoomType { Theory , Lab , Auditorium };
class Room {
private:
        std::string m_roomId;
        int m_capacity;
        RoomType m_type;
        std::string m_building;

    public:
        Room(std::string roomId, int capacity, RoomType type, std::string building = "");
        std::string getRoomId() const;
        int getCapacity() const;
        RoomType getType() const;
        std::string getTypeAsString() const;
        std::string getBuilding() const;
};


#endif