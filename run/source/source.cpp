import std;
import rigid;

int main()
{
    auto x = std::filesystem::current_path();
          auto file   = std::ifstream{ "s.png", std::ios::binary };
    const auto string = std::string{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    const auto image  = std::span<const std::uint8_t>{ std::bit_cast<const std::uint8_t*>(string.data()), string.size() };
    const auto result = rgd::decode(image);

    return 0;
}
