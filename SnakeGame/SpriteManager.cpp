#include "stdafx.h"
#include "SpriteManager.h"

SpriteManager* SpriteManager::instance_ = 0;

void SpriteManager::CreateInstance() {
	if (instance_ != 0) return;
	instance_ = new SpriteManager();
	instance_->bDebug = false;
}

void SpriteManager::DestroyInstance() {
	if (instance_ == 0) return;
	{
		std::map<std::string, SDL_Texture*>::iterator iter = instance_->texture_map_.begin();
		while (iter != instance_->texture_map_.end())
		{
			SDL_DestroyTexture(iter->second);
			iter++;
		}
		instance_->texture_map_.clear();
	}
	{
		// std::map<std::string, std::vector<SDL_Rect*>*> sprite_rects_;
		std::map<std::string, std::vector<SDL_Rect>*>::iterator iter = instance_->sprite_rects_.begin();
		while (iter != instance_->sprite_rects_.end())
		{
			if (iter->second != nullptr)
			{
				iter->second->clear();
				iter->second->shrink_to_fit();
				delete iter->second;
			}
			iter->second = nullptr;
			iter++;
		}
		instance_->sprite_rects_.clear();
	}
	delete(instance_);
}

SpriteManager* SpriteManager::GetSingleton(){
	return instance_;
}

// 배경과 같이 정적인이미지의 로드 함수
bool SpriteManager::LoadTexture(std::string file_id, std::string file_name){ // textrue
	if (instance_->texture_map_[file_id] != nullptr)
		return true;

	SDL_Surface* temp_surface = IMG_Load(file_name.c_str());
	if (temp_surface == 0)
		return false;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, temp_surface);
	SDL_FreeSurface(temp_surface);
	if (texture != 0)
	{
		instance_->texture_map_[file_id] = texture;
		return true;
	}
	
	return false;
}

// 캐릭터 같이 동적 움직임을 가지는 load 방법
bool SpriteManager::LoadTexture(std::string file_id, std::string file_name, unsigned char color_key_r, unsigned char color_key_g, unsigned char color_key_b) {
	if (instance_->texture_map_[file_id] != nullptr)
		return true;

	SDL_Surface* temp_surface = IMG_Load(file_name.c_str());
	if (temp_surface == 0)
		return false;
	SDL_SetColorKey(temp_surface, SDL_TRUE, SDL_MapRGB(temp_surface->format, color_key_r, color_key_g, color_key_b));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, temp_surface);
	
	SDL_FreeSurface(temp_surface);

	if (texture != 0)
	{
		instance_->texture_map_[file_id] = texture;
		return true;
	}
	return false;
}

// 이미지 파일에서 선택할 영역을 미리 설정해주는 함수
void SpriteManager::AddSpriteRect(std::string sprite_id, SDL_Rect* rect) {
	if (instance_->sprite_rects_[sprite_id] == nullptr)
		instance_->sprite_rects_[sprite_id] = new std::vector<SDL_Rect>();
	instance_->sprite_rects_[sprite_id]->push_back(*rect);
	delete rect;
	rect = nullptr;
}

void SpriteManager::DrawSprite(SDL_FRect* drawRect, SpriteInfo* info) {
	if (info == nullptr ||
		info->brender == false ||
		instance_->texture_map_[info->file_id]  == nullptr ||
		instance_->sprite_rects_[info->sprite_id] == nullptr ||
		instance_->sprite_rects_[info->sprite_id]->size() <= 0)
		return;

	SDL_SetTextureColorMod(instance_->texture_map_[info->file_id], info->color.r, info->color.g, info->color.b); // drawTexture(renderer, 0, 0, texture);

	SDL_Rect* srcrect = &(*instance_->sprite_rects_[info->sprite_id])[info->currentIndex];

	SDL_FRect targetrect = { 
		drawRect->x , 
		drawRect->y  ,
		drawRect->w * info->sizeX,
		drawRect->h * info->sizeY };

	SDL_RenderCopyExF(g_renderer, instance_->texture_map_[info->file_id],
		srcrect, &targetrect,
		0.0f, nullptr, info->flipsMode);
	srcrect = nullptr;
	//*info->drawRect = targetrect;
	DrawDebugBorder(&targetrect);

	if (info->spriteSpeed > 0)	{
		Uint32 time = SDL_GetTicks();
		if (time - info->currenttime < (info->spriteSpeed * 1000))
			return;
		else
			info->currenttime = time;
	}
	if (info->bend == true)
		return;
	info->currentIndex++;
	if (info->currentIndex >= instance_->sprite_rects_[info->sprite_id]->size())
	{
		if (info->bloop == false)
		{
			info->bend = true;
		}
		info->currentIndex = 0;
	}
}

void SpriteManager::DrawDebugRect(SDL_FRect* info, float sizeX , float sizeY, SDL_Color color)
{
	if (instance_->bDebug == false)
		return;
	SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
	SDL_FRect temp = *info;
	temp.w *= sizeX;
	temp.h *= sizeY;
	SDL_RenderFillRectF(g_renderer, &temp);
	SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 0);
}

void SpriteManager::DrawDebugRect(SDL_FRect* info, SDL_Color color)
{
	if (instance_->bDebug == false)
		return;
	SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRectF(g_renderer, info);
	SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 0);
}

void SpriteManager::DrawDebugBorder(SDL_FRect* info, float sizeX, float sizeY, SDL_Color color)
{
	if (instance_->bDebug == false)
		return;
	SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a);

	SDL_FRect temp = *info;
	temp.w *= sizeX;
	temp.h *= sizeY;
	SDL_RenderDrawRectF(g_renderer, &temp);
	SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 0);
}

void SpriteManager::DrawDebugBorder(SDL_FRect* info, SDL_Color color)
{
	if (instance_->bDebug == false)
		return;
	SDL_SetRenderDrawColor(g_renderer, color.r, color.g, color.b, color.a); 
	SDL_RenderDrawRectF(g_renderer, info);
	SDL_SetRenderDrawColor(g_renderer, 0,0,0,0);
}