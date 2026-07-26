#include "ObfStr.h"

bool showMenu = true;
bool bFullChecked = false;
int selectedFeatures = 1;
android_app *i_App = 0;

unsigned int gpCrash = 0xfa91b9cd;
static int crash(int randomval){
    volatile int *p = (int *)gpCrash;
    p += randomval;
    p += *p + randomval;
    p = 0;
    p += *p;
    return *p;
}

void CenteredText(ImColor color, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const char *text, *text_end;
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text, text_end).x) * 0.5);
    ImGui::TextColoredV(color, fmt, args);
    va_end(args);
}

inline ImVec4 RGBA2ImVec4(int r, int g, int b, int a) {
    float newr = (float)r / 255.0f;
    float newg = (float)g / 255.0f;
    float newb = (float)b / 255.0f;
    float newa = (float)a / 255.0f;
    return ImVec4(newr, newg, newb, newa);
}

void HideMenu(bool& bShow) {
    if (bShow) {
        ImGui::OpenPopup("ConfirmHide");
    }

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("ConfirmHide", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Are you sure you want to hide the menu?");
        if (ImGui::Button("Yes", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0)))
        {
            showMenu = false;
            bShow = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            bShow = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void loadConfig() {
    int fd = open("/storage/emulated/0/Android/data/com.mobile.legends/tmh.ini", O_RDONLY);
        read(fd, &Config, sizeof(Config));
        read(fd, &Aim, sizeof(Aim));
        read(fd, &SetFieldOfView, sizeof(SetFieldOfView));
        close(fd);
}
void saveConfig(){
    int fd = open("/storage/emulated/0/Android/data/com.mobile.legends/tmh.ini", O_WRONLY | O_CREAT, 0644);
    system("chmod 777 /storage/emulated/0/Android/data/com.mobile.legends/tmh.ini");
    write(fd, &Config , sizeof(Config));
    write(fd, &Aim, sizeof(Aim));
    write(fd, &SetFieldOfView, sizeof(SetFieldOfView));
    close(fd);
}

void loadCFG(){
    loadConfig();
}
void saveCFG(){
    saveConfig();
}

struct sTheme {
    bool TemaAsik;
    bool Color;
};
sTheme Theme{0};

std::string msg;

void LoginThread(const std::string &user_key, bool *success) {
    msg = Login(g_vm, user_key.c_str(), success);
}

bool selectedThemes;

inline ImColor main_color(230, 134, 224, 255);

inline ImColor text_color[3] = {ImColor(255, 255, 255, 255), ImColor(200, 200, 200, 255), ImColor(150, 150, 150, 255) };

inline ImColor foreground_color(1.f, 1.f, 1.f, 0.15f);

inline ImColor background_color(13, 14, 16, 200);

inline ImColor second_color(255, 255, 255, 20);

inline ImColor stroke_color(35, 35, 35, 255);

inline ImColor child_color(19, 19, 19, 255);

inline ImColor scroll_bg_col(24, 24, 24, 255);

inline ImColor winbg_color(15, 16, 18, 200);

inline ImFont* icon_font;

inline ImFont* logo_font;

inline ImFont* icon_big_font;

inline ImFont* medium_font;

inline ImFont* small_font;

inline ImFont* small_icon_font;

inline ImFont* arrow_icons;

inline float anim_speed = 8.f;

static inline ImDrawList* foremenu_drawlist;

inline ImColor GetColorWithAlpha(ImColor color, float alpha)
{
return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

inline ImVec2 center_text(ImVec2 min, ImVec2 max, const char* text)
{
    return min + (max - min) / 2 - ImGui::CalcTextSize(text) / 2;
}

inline int rotation_start_index;
inline void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

inline ImVec2 ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}

inline void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

void Trinage_background()
{

    ImVec2 screen_size = ImVec2(glWidth, glHeight);

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_size[100];
    static float partile_radius[100];
    static float partile_rotate[100];

    for (int i = 1; i < 50; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = screen_size.y + 30;
            partile_speed[i] = 70 + rand() % 135;
            partile_radius[i] = 2 + rand() % 10;
            partile_size[i] = 15 + rand() % 40;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = -50;
        }

        partile_pos[i].y -= ImGui::GetIO().DeltaTime * partile_speed[i];
        partile_rotate[i] -= ImGui::GetIO().DeltaTime;

        if (partile_pos[i].y < 10)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
            partile_rotate[i] = 0;
        }

        ImRotateStart();
        ImGui::GetWindowDrawList()->AddRectFilled(partile_pos[i] - ImVec2(partile_size[i], partile_size[i]), partile_pos[i] + ImVec2(partile_size[i], partile_size[i]), GetColorWithAlpha(main_color, 0.3f), 5.f);
        ImRotateEnd(partile_rotate[i]);
    }
}
int selectedOption = 0;

// Faux-bold text: overdraw the glyphs with a sub-pixel horizontal offset so a
// single (non-bold) font still renders a visibly heavier weight.
inline void TextBoldColored(const ImVec4& color, const char* text) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImU32 col = ImGui::ColorConvertFloat4ToU32(color);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddText(pos, col, text);
    dl->AddText(ImVec2(pos.x + 0.8f, pos.y), col, text);
    ImGui::Dummy(ImGui::CalcTextSize(text));
}

void DrawMenu() {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);

    ImGuiIO& io = ImGui::GetIO();

    static float window_scale;
    if (!window_scale) window_scale = 1.0f;
    io.FontGlobalScale = window_scale;

    static bool isLogin = false, isSave = false;
    static char s[64];
    if (isLogin && !isSave) {
        SharedPreferences sharedPref(GetJNIEnv(g_vm), "xyourzone_sharedpref");
        SharedPreferences_Editor editor=sharedPref.edit();
        editor.putString("key", s);
        editor.commit();
        isSave = true;
    }

    static bool isPopUpHide = false;
    HideMenu(isPopUpHide);
    
    static bool bFlagAutoResize = true;
    static ImGuiWindowFlags window_flags;
    if (bFlagAutoResize) {
        window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    } else {
        window_flags = ImGuiWindowFlags_None;
    }
    
    if (isLogin) {
        loadBattleData(battleData);
        bFullChecked = true;
    }
	
	std::string XYOURZONE;
    
	if (inVip == "100"){
		XYOURZONE = std::string("VIP VERSION ");
	} else {
		XYOURZONE = std::string("FREE VERSION ");
	}
	
    std::string FULLTITLE = std::string("TMH") + std::string(" | ") + clientManager.c_str() + std::string(" | ") + std::string(ABI);
    if (!ImGui::Begin(FULLTITLE.c_str(), 0, window_flags)) {
        ImGui::End();
        return;
    }
	
    using namespace ImGui;
	ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.3f, (float) glHeight * 0.5f),ImGuiCond_Once); // 45% width 70% height
	
	// revjump bypass !isLogin to isLogin visual hack
	if (!isLogin) {
        if (ImGui::BeginTabBar("TabLogin", ImGuiTabBarFlags_FittingPolicyScroll)) {
            if (ImGui::BeginTabItem("Login Menu")) {
                ImGui::BeginGroupPanel("Please Login! (Copy Key to Clipboard)", ImVec2(0.0f, 0.0f)); {
					
					ImGui::Spacing();
					ImGui::Spacing();
                    ImGui::PushItemWidth(-1);
                    ImGui::InputText("##key", s, sizeof s);
                    ImGui::PopItemWidth();
					
                    if (ImGui::Button("Paste Key", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))) {
                        auto key = getClipboardText(g_vm);
                        strncpy(s, key.c_str(), sizeof s);
                    }

                    ImGui::SameLine();

                    static std::string err;
                    if (ImGui::Button("Load Saved Key", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        SharedPreferences sharedPref(GetJNIEnv(g_vm), "xyourzone_sharedpref");
                        auto key = sharedPref.getString("key");
                        strncpy(s, key.c_str(), sizeof s);
                    }

                    if (ImGui::Button("Login", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        std::thread login_thread(LoginThread, std::string(s), &isLogin);
                        login_thread.detach();
                    }
					ImGui::Spacing();
					ImGui::Spacing();
					// Seller contact: "BUY KEY @safkq" (handle in bold) + Telegram link.
					ImGui::Text("BUY KEY");
					ImGui::SameLine(0.0f, 6.0f);
					TextBoldColored(RGBA2ImVec4(0, 200, 255, 255), "@safkq");
					if (ImGui::Button("Contact @safkq on Telegram", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						openURL(g_vm, OBF(KS_TELEGRAM_URL));
					}
                    ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "%s", msg.c_str());
                    ImGui::Spacing();
                }
                ImGui::EndGroupPanel();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    } else {
		if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_FittingPolicyScroll)) {
			if (selectedFeatures == 1 | selectedFeatures == 2){
				if (ImGui::BeginTabItem("ESP")) {
            	if (ImGui::CollapsingHeader("Player")) {
                	if (ImGui::BeginTable("ESPPlayer", 3)) {
                    	ImGui::TableNextColumn();	ImGui::Checkbox(" Player Line", &Config.ESP.Player.Line);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Box", &Config.ESP.Player.Box);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Name", &Config.ESP.Player.Name);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Hero", &Config.ESP.Player.Hero);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Health", &Config.ESP.Player.Health);
						ImGui::TableNextColumn();	ImGui::Checkbox(" Player Distance", &Config.ESP.Player.Distance);
						ImGui::TableNextColumn();	ImGui::Checkbox(" Player Locator", &Config.ESP.Player.Locator2);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Hero Alert", &Config.ESP.Player.Alert);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Icon Hero", &Config.ESP.Player.HeroZ);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Visible Check", &Config.ESP.Player.Visible);
                        ImGui::TableNextColumn();	ImGui::Spacing();
                        ImGui::EndTable();
                    }
                }
                if (ImGui::CollapsingHeader("Monster")) {
                	if (ImGui::BeginTable("Monster", 2)) {
                    	ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Round", &Config.ESP.Monster.Rounded);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Health", &Config.ESP.Monster.Health);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Name", &Config.ESP.Monster.Name);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Alert", &Config.ESP.Monster.Alert);
						ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Icon", &Config.ESP.Monster.Locator);
						ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Locator", &Config.ESP.Monster.Locator2);
						ImGui::TableNextColumn();	ImGui::Checkbox(" Monster UID", &Config.m_IDConf);
                        ImGui::EndTable();
                    }
                }
                ImGui::EndTabItem();
			}
			}
			if (selectedFeatures == 1 | selectedFeatures == 2){
				if (ImGui::BeginTabItem("Maphack")) {
                ImGui::Checkbox("Minimap Icon", &Config.MinimapIcon);
                if (!Config.MinimapIcon) ImGui::BeginDisabled();
                ImGui::SameLine();
                ImGui::Checkbox("Hide Line", &Config.HideLine);
                ImGui::BeginGroup();
                {
                    ImGui::BeginGroupPanel("MiniMap Adjustable", ImVec2(-1.0f, 0.0f));
                   {
                        ImGui::BeginGroupPanel("Map Position", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderFloat("##MapPosition", &StartPos.x, 0.0f, (float)(screenWidth / 2));
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();

                        ImGui::BeginGroupPanel("Map Size", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderInt("##MapSize", &MapSize, 0, 800);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();

                        ImGui::BeginGroupPanel("Icon Size", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderInt("##IconSize", &ICSize, 0, 100);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();

                        ImGui::BeginGroupPanel("Health Thin", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderInt("##HealthThin", &ICHealthThin, 0, 10);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
						
						ImGui::BeginGroupPanel("Health Color", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::ColorEdit3("##HealthColor", ColorHealth);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
														     
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();
                    if (!Config.MinimapIcon) ImGui::EndDisabled();
                }
                ImGui::EndGroup();
                ImGui::EndTabItem();
            }
			}
			
			static int SelectInfo = 0;
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
            if (ImGui::BeginTabItem("Setting")) {
                ImGui::BeginGroupPanel("Menu Setting", ImVec2(-1.0f, 0.0f));
                {
                    ImGui::Checkbox("Auto Resize", &bFlagAutoResize);
                    ImGui::BeginGroupPanel("Window Size", ImVec2(-1.0f, 0.0f));
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat("##Scale", &window_scale, 0.5f, 2.5f, "%.1f");
                        ImGui::PopItemWidth();
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();
                    
                    if (ImGui::Button("Hide Menu", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        isPopUpHide = true;
                    }

                    ImGui::BeginGroupPanel("English", ImVec2(-1.0f, 0.0f));
                    {
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "To display the menu again,");
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "simply touch on the lower left corner of your screen.");
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();

                    ImGui::BeginGroupPanel("Indonesia", ImVec2(-1.0f, 0.0f));
                    {
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "Untuk menampilkan kembali menu,");
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "cukup sentuh di pojok kiri bawah layar Anda.");
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();
                    
                    ImGui::Spacing();
                    
                    if (ImGui::Button("Save Cheat Setting", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))) {
                        saveCFG();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Load Cheat Setting", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        loadCFG();
                    }
					
        			ImGui::Separator();
					
					ImGui::Spacing();
                    ImGui::BeginGroupPanel("Key Info", ImVec2(-1.0f, 0.0f));
                    {
						Text("Telegram Group: "); SameLine();
                        TextColored(RGBA2ImVec4(176, 40, 40, 255), "@THEMAPHACK");
                        Text("Key Expired: "); SameLine();
                        ImGui::TextColored(RGBA2ImVec4(176, 40, 40, 255), expired.c_str());
                        Text("Key Slot: "); SameLine();
                        TextColored(RGBA2ImVec4(176, 40, 40, 255), slotZ.c_str());
                        Text("Mod Status: "); SameLine();
                        TextColored(RGBA2ImVec4(176, 40, 40, 255), "Safe (5K Server)");
						ImGui::Text("Vip :"); SameLine();
						if (inVip == "100"){
							TextColored(RGBA2ImVec4(176, 40, 40, 255), "Yes");
						} else {
							TextColored(RGBA2ImVec4(176, 40, 40, 255), "No");
						}
						
						Text("Telegram Channel: "); SameLine();
						Text("@THEMAPHACK");
                    }
                    ImGui::EndGroupPanel();
                }
                ImGui::EndGroupPanel();
                ImGui::EndTabItem();
            }
			
			ImGui::EndTabBar();
		}
		ImGui::Separator();
        ImGui::TreePop();
	}
}
