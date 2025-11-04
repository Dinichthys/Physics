#include "graphics.hpp"

#include <stdlib.h>
#include <stdexcept>
#include <string.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"

namespace graphics {

//-----------------FONT---------------------------------------------------------------------------------------

    class FontImpl : public sf::Font {
        public:
            explicit FontImpl()
                :sf::Font() {};

            virtual bool loadFromFile(const std::string& path) {
                return sf::Font::loadFromFile(path);
            };
    };

    Font::Font() {
        impl_ = new FontImpl;
    };

    void Font::loadFromFile(const std::string& path) const {
        if (impl_->loadFromFile(path) == NULL) {
            throw std::runtime_error("No files for font uploading");
        }
    };

//------------------------------------------------------------------------------------------------------------

//-----------------TEXT---------------------------------------------------------------------------------------

    Text::Text(const std::string& text, const std::string& font_file_name, unsigned char height) {
        font_ = new(std::nothrow) sf::Font();
        if (font_ == NULL) {
            throw std::runtime_error("Can't create font for Text object\n");
        }

        if (strcmp(font_file_name.c_str(), "") != 0) {
            if (!((sf::Font*)font_)->loadFromFile(font_file_name)) {
                throw std::runtime_error("No such file with fonts");
            }
        }

        text_ = new(std::nothrow) sf::Text(text, *((sf::Font*)font_), height);
        if (text_ == NULL) {
            throw std::runtime_error("Can't create Text object\n");
        }
    }

    Text::Text(const Text& other) {
        font_ = new(std::nothrow) sf::Font(*((sf::Font*)other.font_));
        if (font_ == NULL) {
            throw std::runtime_error("Can't create font for Text object\n");
        }

        text_ = new(std::nothrow) sf::Text(*((sf::Text*)other.text_));
        if (text_ == NULL) {
            throw std::runtime_error("Can't create Text object\n");
        }

        ((sf::Text*)text_)->setFont(*((sf::Font*)font_));
    }

    Text::~Text() {
        delete ((sf::Text*)text_);
        delete ((sf::Font*)font_);
    }

    void Text::SetPosition(const Coordinates& lt_corner) const {
        ((sf::Text*)text_)->setPosition({lt_corner[0], lt_corner[1]});
    }

    void Text::SetText(const std::string& text) {
        ((sf::Text*)text_)->setString(text);
    }

    void Text::SetFont(const std::string& font) {
        ((sf::Font*)font_)->loadFromFile(font);
        ((sf::Text*)text_)->setFont(*((sf::Font*)font_));
    }

    dr4::Rect2f Text::GetBounds() const {
        sf::FloatRect rect = ((sf::Text*)text_)->getLocalBounds();
        return dr4::Rect2f(dr4::Vec2f(rect.getPosition().x, rect.getPosition().y),
                           dr4::Vec2f(rect.getSize().x, rect.getSize().y));
    }

//-----------------VERTEX ARRAY-------------------------------------------------------------------------------

    VertexArray::VertexArray(size_t size) {
        vertex_array_ = new(std::nothrow) sf::VertexArray(sf::PrimitiveType::Points, size);
        if (vertex_array_ == NULL) {
            throw std::runtime_error("Can't create VertexArray\n");
        }
    }

    VertexArray::VertexArray(const VertexArray& other) {
        vertex_array_ = new(std::nothrow) sf::VertexArray(*((sf::VertexArray*)other.GetVertexArray()));
        if (vertex_array_ == NULL) {
            throw std::runtime_error("Can't create VertexArray\n");
        }
    }

    VertexArray::~VertexArray() {
        delete ((sf::VertexArray*)vertex_array_);
    }

    void VertexArray::SetPixelPosition(size_t index, const Coordinates& pos) const {
        (*((sf::VertexArray*)vertex_array_))[index].position = {pos[0], pos[1]};
    }
    void VertexArray::SetPixelColor(size_t index, const Color& color) const {
        (*((sf::VertexArray*)vertex_array_))[index].color =
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness());
    }

//-----------------RECTANGLE SHAPE----------------------------------------------------------------------------

    RectangleShape::RectangleShape(const dr4::Vec2f& pos, const dr4::Vec2f& size)
        :dr4::Rectangle(dr4::Rect2f(pos, size), dr4::Color(0, 0, 0, 0))  {
        rectangle_ = new(std::nothrow) sf::RectangleShape(sf::Vector2f(size.x, size.y));
        if (rectangle_ == NULL) {
            throw std::runtime_error("Can't create RectangleShape\n");
        }
        ((sf::RectangleShape*)rectangle_)->setPosition({pos.x, pos.y});
    }

    RectangleShape::RectangleShape(const dr4::Rectangle& rect)
        :dr4::Rectangle(rect) {
        rectangle_ = new(std::nothrow) sf::RectangleShape(sf::Vector2f(rect.rect.size.x, rect.rect.size.y));
        if (rectangle_ == NULL) {
            throw std::runtime_error("Can't create RectangleShape\n");
        }
    }

    RectangleShape::RectangleShape(const RectangleShape& other) {
        rectangle_ = new(std::nothrow) sf::RectangleShape(*((sf::RectangleShape*)other.GetRectangle()));
        if (rectangle_ == NULL) {
            throw std::runtime_error("Can't create RectangleShape\n");
        }
    }

    RectangleShape::~RectangleShape() {
        delete ((sf::RectangleShape*)rectangle_);
    }

    void RectangleShape::SetSize(float width, float height) {
        ((sf::RectangleShape*)rectangle_)->setSize({width, height});
        rect.size = {width, height};
    }

    void RectangleShape::SetPosition(const Coordinates& lt_corner) {
        ((sf::RectangleShape*)rectangle_)->setPosition(lt_corner[0], lt_corner[1]);
        rect.pos = {lt_corner[0], lt_corner[1]};
    }

    void RectangleShape::SetRotation(float angle) const {
        ((sf::RectangleShape*)rectangle_)->setRotation(angle);
    }
    void RectangleShape::Rotate(float angle) const {
        ((sf::RectangleShape*)rectangle_)->rotate(angle);
    }

    void RectangleShape::SetOutlineColor(const graphics::Color color) {
        ((sf::RectangleShape*)rectangle_)->setOutlineColor(
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness()));
        borderColor = color;
    }
    void RectangleShape::SetFillColor(const graphics::Color color) {
        ((sf::RectangleShape*)rectangle_)->setFillColor(
            sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness()));
        fill = color;
    }

//-----------------IMAGE--------------------------------------------------------------------------------------

    Image::Image(float width, float height)
        :dr4::Image(width, height) {
        image_ = new sf::Image();
        ((sf::Image*) image_)->create(width, height);
        width_ = width;
        height_ = height;
    }

    Image::~Image() {
        delete ((sf::Image*) image_);
    }

    void Image::SetPixel(unsigned x, unsigned y, Color color) {
        ((sf::Image*) image_)->setPixel(x, y,
             sf::Color(color.GetRedPart(), color.GetGreenPart(), color.GetBluePart(), color.GetBrightness()));
    }

    void Image::SetPixel(unsigned x, unsigned y, dr4::Color color) {
        ((sf::Image*) image_)->setPixel(x, y, sf::Color(color.r, color.b, color.g, color.a));
    }

    dr4::Color Image::GetPixel(unsigned x, unsigned y) const {
        sf::Color color = ((sf::Image*) image_)->getPixel(x, y);
        return dr4::Color(color.r, color.b, color.g, color.a);
    }

    Color Image::GetPixelColor(unsigned x, unsigned y) const {
        return Color(GetPixel(x, y));
    }

    void Image::SetSize(dr4::Vec2f size) {
        width_ = size.x;
        height_ = size.y;
    }

    dr4::Vec2f Image::GetSize() const {
        return dr4::Vec2f(width_, height_);
    }

    float Image::GetWidth() const {
        return width_;
    }

    float Image::GetHeight() const {
        return height_;
    }

//-----------------TEXTURE------------------------------------------------------------------------------------

    Texture::Texture(float width, float height) {
        texture_ = new sf::RenderTexture();
        ((sf::RenderTexture*) texture_)->create(width, height);
        width_ = width;
        height_ = height;
    }

    void Texture::SetSize(dr4::Vec2f size) {
        width_ = size.x;
        height_ = size.y;
    }

    dr4::Vec2f Texture::GetSize() const {
        return dr4::Vec2f(width_, height_);
    }

    float Texture::GetWidth() const {
        return width_;
    }

    float Texture::GetHeight() const {
        return height_;
    }

    void Texture::Draw(const dr4::Rectangle &rect) {
        sf::RectangleShape sf_rect({rect.rect.size.x, rect.rect.size.y});
        sf_rect.setFillColor(sf::Color(rect.fill.r, rect.fill.g, rect.fill.g, rect.fill.a));
        sf_rect.setSize({rect.rect.pos.x, rect.rect.pos.y});
        sf_rect.setOutlineColor(sf::Color(rect.borderColor.r, rect.borderColor.g, rect.borderColor.b, rect.borderColor.a));
        ((sf::RenderTexture*)texture_)->draw(sf_rect);
    }

    void Texture::Draw(const dr4::Text &text) {
        sf::Text sf_text(sf::String(text.text), *((dynamic_cast<const Font*>(text.font))->GetFont()), text.fontSize);
       ((sf::RenderTexture*)texture_)->draw(sf_text);
    }

    void Texture::Draw(const dr4::Image &img, const dr4::Vec2f &pos) {
        sf::Texture txtr(((sf::RenderTexture*)texture_)->getTexture());
        txtr.update(*((sf::Image*)(dynamic_cast<const Image&>(img).GetImage())), pos.x, pos.y);
        sf::Sprite sprite(txtr);
        ((sf::RenderTexture*)texture_)->draw(sprite);
    }
    void Texture::Draw(const dr4::Texture &texture, const dr4::Vec2f &pos) {
        sf::Sprite sprite(((sf::RenderTexture*)(dynamic_cast<const Texture&>(texture).texture_))->getTexture());
        sprite.setPosition({pos.x, pos.y});
        ((sf::RenderTexture*)texture_)->draw(sprite);
    }

//-----------------RENDER WINDOW------------------------------------------------------------------------------

    RenderWindow::RenderWindow(size_t width, size_t height, const char* window_name)
        :title_(window_name) {
        width_ = width;
        height_ = height;
        window_ = new(std::nothrow) sf::RenderWindow(sf::VideoMode(width, height), window_name);
        if (window_ == NULL) {
            throw std::runtime_error("Can't open a window\n");
        }
    }

    RenderWindow::~RenderWindow() {
        delete ((sf::RenderWindow*)window_);
    }

    std::optional<dr4::Event> RenderWindow::PollEvent() {
        sf::Event sf_event;
        if (!(((sf::RenderWindow*)window_)->pollEvent(sf_event))) {
            return {};
        }

        dr4::Event event;

        switch (sf_event.type) {
            case sf::Event::Closed : {
                event.type = dr4::Event::Type::QUIT;
                break;
            }
            case sf::Event::MouseButtonPressed : {
                event.type = dr4::Event::Type::MOUSE_DOWN;
                switch(sf_event.mouseButton.button) {
                    case sf::Mouse::Button::Left :
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_LEFT;
                        break;
                    case sf::Mouse::Button::Right :
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_RIGHT;
                        break;
                    case sf::Mouse::Button::Middle :
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_MIDDLE;
                        break;
                    default:
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_UNKNOWN;
                }
                Coordinates pos(GetMousePos());
                event.mouseButton.pos = {pos[0], pos[1]};
                break;
            }
            case sf::Event::MouseButtonReleased : {
                event.type = dr4::Event::Type::MOUSE_UP;
                switch(sf_event.mouseButton.button) {
                    case sf::Mouse::Button::Left :
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_LEFT;
                        break;
                    case sf::Mouse::Button::Right :
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_RIGHT;
                        break;
                    case sf::Mouse::Button::Middle :
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_MIDDLE;
                        break;
                    default:
                        event.mouseButton.button = dr4::MouseCode::MOUSECODE_UNKNOWN;
                }
                Coordinates pos(GetMousePos());
                event.mouseButton.pos = {pos[0], pos[1]};
                break;
            }
            case sf::Event::MouseMoved : {
                event.type = dr4::Event::Type::MOUSE_MOVE;
                Coordinates pos(GetMousePos());
                event.mouseMove.pos = {pos[0], pos[1]};
                break;
            }
            case sf::Event::KeyPressed : {
                event.type = dr4::Event::Type::KEY_DOWN;
                switch(sf_event.key.scancode) {
                    case sf::Keyboard::Scan::Scancode::A : {
                        event.key.sym = dr4::KeyCode::KEYCODE_A;
                        break;
                    }
                    case sf::Keyboard::Scan::Scancode::D : {
                        event.key.sym = dr4::KeyCode::KEYCODE_D;
                        break;
                    }
                    case sf::Keyboard::Scan::Scancode::S : {
                        event.key.sym = dr4::KeyCode::KEYCODE_S;
                        break;
                    }
                    case sf::Keyboard::Scan::Scancode::W : {
                        event.key.sym = dr4::KeyCode::KEYCODE_W;
                        break;
                    }

                    case sf::Keyboard::Scan::Scancode::Right : {
                        event.key.sym = dr4::KeyCode::KEYCODE_RIGHT;
                        break;
                    }
                    case sf::Keyboard::Scan::Scancode::Left : {
                        event.key.sym = dr4::KeyCode::KEYCODE_LEFT;
                        break;
                    }
                    case sf::Keyboard::Scan::Scancode::Up : {
                        event.key.sym = dr4::KeyCode::KEYCODE_UP;
                        break;
                    }
                    case sf::Keyboard::Scan::Scancode::Down : {
                        event.key.sym = dr4::KeyCode::KEYCODE_DOWN;
                        break;
                    }
                    default : {
                        event.key.sym = dr4::KeyCode::KEYCODE_UNKNOWN;
                        break;
                    }
                }
                break;
            }
            default : {
                event.type = dr4::Event::Type::UNKNOWN;
                break;
            }
        }

        return event;
    }

    float RenderWindow::GetWidth() const {
        return ((sf::RenderWindow*)window_)->getSize().x;
    };

    float RenderWindow::GetHeight() const {
        return ((sf::RenderWindow*)window_)->getSize().y;
    };

    dr4::Vec2f RenderWindow::GetSize() const {
        return {((sf::RenderWindow*)window_)->getSize().x, ((sf::RenderWindow*)window_)->getSize().y};
    }

    void RenderWindow::SetSize(const ::dr4::Vec2f& size) {
        width_ = size.x;
        height_ = size.y;
        ((sf::RenderWindow*)window_)->setSize({width_, height_});
    }

    void RenderWindow::SetTitle(const std::string &title) {
        ((sf::RenderWindow*)window_)->setTitle(title);
        title_ = title;
    }

    const std::string &RenderWindow::GetTitle() const {
        return title_;
    }

    dr4::Texture *RenderWindow::CreateTexture() {
        sf::Texture texture;
        texture.create(width_, height_);
        texture.update(*((sf::Image*)(dynamic_cast<Image*>(CreateImage())->GetImage())));

        Texture* my_texture = new Texture(width_, height_);
        sf::RenderTexture* ptr = (sf::RenderTexture*)my_texture->GetTexture();
        sf::Sprite sprite(texture);
        ptr->draw(sprite);

        return my_texture;
    }

    dr4::Image *RenderWindow::CreateImage() {
        Image* image = new Image(width_, height_);
        sf::Image* sf_image = (sf::Image*)image->GetImage();
        *sf_image = ((sf::RenderWindow*)window_)->capture();
        return image;
    }

    Coordinates RenderWindow::GetMousePos() const {
        float scale_x = GetWidth() / width_;
        float scale_y = GetHeight() / height_;
        sf::RenderWindow* render = (sf::RenderWindow*)window_;
        return Coordinates(2, (float)sf::Mouse::getPosition(*render).x / scale_x,
                              (float)sf::Mouse::getPosition(*render).y / scale_y);
    }

    void RenderWindow::Draw(const graphics::RectangleShape& rect) const {
        ((sf::RenderWindow*)window_)->draw(*((sf::RectangleShape*)rect.GetRectangle()));
    }
    void RenderWindow::Draw(const graphics::VertexArray& arr) const {
        ((sf::RenderWindow*)window_)->draw(*((sf::VertexArray*)arr.GetVertexArray()));
    }
    void RenderWindow::Draw(const graphics::Text& text) const {
        ((sf::RenderWindow*)window_)->draw(*((sf::Text*)text.GetText()));
    }

    void RenderWindow::Draw(const dr4::Texture &texture, dr4::Vec2f pos) {
        sf::Sprite sprite(((sf::RenderTexture*)(dynamic_cast<const Texture&>(texture).GetTexture()))->getTexture());
        sprite.setPosition({pos.x, pos.y});
        ((sf::RenderWindow*)window_)->draw(sprite);
    }

    void RenderWindow::Display() {
        ((sf::RenderWindow*)window_)->display();
    }

    bool RenderWindow::IsOpen() const {
        return ((sf::RenderWindow*)window_)->isOpen();
    }

    void RenderWindow::Close() {
        ((sf::RenderWindow*)window_)->close();
    }

    void RenderWindow::Clear(const dr4::Color &color) {
        ((sf::RenderWindow*)window_)->clear(sf::Color(color.r, color.g, color.b, color.a));
    }

//------------------------------------------------------------------------------------------------------------

}
