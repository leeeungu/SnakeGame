#include "stdafx.h"
#include "TextManager.h"

TextManager* TextManager::instance_ = 0;

void TextManager::CreateInstance() {
	if (instance_ != 0) return;
	TTF_Init();
	instance_ = new TextManager();
}

void TextManager::DestroyInstance() {
	if (instance_ == 0) return;

	{
		instance_->files_map_.clear();
	}

	delete(instance_);
	TTF_Quit();
}

TextManager* TextManager::GetSingleton() {
	return instance_;
}

void TextManager::LoadFont(std::string font_id, std::string font_files)
{
	instance_->files_map_[font_id] = font_files;
}

void TextManager::RenderFont(TextInfo* info, std::string str) 
{
	// id에 해당되는 font가 없으면 그리기 중지
	if (instance_->files_map_[info->font_id].empty())
		return;

	// font file을 연다.
	TTF_Font* font1 = TTF_OpenFont((instance_->files_map_[info->font_id]).c_str(), info->font_size);
	// 해당 파일이 없으면 return;
	if (font1 == 0)
		return;

	SDL_Surface* tmp_surface;
		//tmp_surface = TTF_RenderUTF8_Blended(font1, CW2A(info->fun(str).c_str(), CP_UTF8), info->font_color);
		//tmp_surface = TTF_RenderText_Blended(font1, CT2CA(info->fun(str).c_str()), info->font_color);
	//if (info->isKorea) // 한국어 일때 인코딩 방법
		tmp_surface = TTF_RenderUTF8_Blended(font1, str.c_str(), info->font_color);
	//else // 그외 인코딩 방법
	//	tmp_surface = TTF_RenderText_Blended(font1, str.c_str(), info->font_color);
	TTF_CloseFont(font1);
	font1 = nullptr;

	// 인코딩 값을 texture에  넣는다..
	SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, tmp_surface);
	SDL_Rect source_rect = { 0,0, tmp_surface->w ,tmp_surface->h };
	SDL_FRect target_rect = { info->target_pos.x, info->target_pos.y, (float)tmp_surface->w,	(float)tmp_surface->h };

	SDL_FreeSurface(tmp_surface);
	tmp_surface = nullptr;

	if (texture == nullptr)
		return;

	// texture 도 있으면 위치에 text를 그린다.
	SDL_RenderCopyF(g_renderer, texture, &source_rect, &target_rect);

	SDL_DestroyTexture(texture);
	texture = nullptr;
}
