#ifndef AnimationManager_h__
#define AnimationManager_h__

#include <map>
#include <string>
#include "AnimationClip.h"
#include "Blender.h"


class AnimationManager
{
public:
	~AnimationManager();
	void Load(double animationSpeed, string file_name, string animationName);
	void LoadAndBlend(double animationSpeed, string file_name_animation1, string file_name_animation2, string animationName);
	void AddAnimationOnQueue(string animation_name);
	void Animate(Model* model, double deltaTime);

private:
	std::map<string,AnimationClip*> m_animationSet;
	AnimationEventController m_animationEventController;
};

void AnimationManager::Load(double animationSpeed, string file_name, string animationName)
{
	if (file_name.empty())
		throw new std::exception("AnimationManager::Load - Insert a Valid Not Empty File Name");

	if (animationName.empty())
		throw new std::exception("AnimationManager::Load - Insert a Valid Not Empty Animation Name");

	AnimationClip* animationLoad = new AnimationClip(animationSpeed, file_name, animationName); //I'm not sure this works

	this->m_animationSet[animationName] = animationLoad;
}

void AnimationManager::LoadAndBlend(double animationSpeed, string file_name_animation1, string file_name_animation2, string animationName){
	
	if (file_name_animation1.empty())
		throw new std::exception("AnimationManager::LoadAndBlend - Insert a Valid Not Empty File Name");
	
	if (file_name_animation2.empty())
		throw new std::exception("AnimationManager::LoadAndBlend - Insert a Valid Not Empty File Name");

	if (animationName.empty())
		throw new std::exception("AnimationManager::LoadAndBlend - Insert a Valid Not Empty Animation Name");

	AnimationClip* animationLoad1 = new AnimationClip(animationSpeed, file_name_animation1, animationName);  
	AnimationClip* animationLoad2 = new AnimationClip(animationSpeed, file_name_animation2, animationName);  

//	this->m_animationSet[animationName] = animationLoad;
}


void AnimationManager::AddAnimationOnQueue(string animation_name)
{
	if (animation_name.empty()) return;

	if ( this->m_animationSet.find(animation_name) == this->m_animationSet.end())
		throw new std::exception("AnimationManager::AddAnimationOnQueue - Animation Name Not Available");

	auto pAnimation = this->m_animationSet[animation_name];

	m_animationEventController.AddAnimation(pAnimation);
}

void AnimationManager::Animate(Model* model, double deltaTime)
{
	AnimationClip* clipToAnimate;
	auto clips = this->m_animationEventController.GetNextAnimations();

	int clipSize = clips.size();

	switch (clipSize)
	{
	case 0:
		return;
		break;
	case 1:
		clipToAnimate = clips[0];
		break;
	default:
		clipToAnimate = Blender::LinearBlend(clips,deltaTime);

		break;
	}
 
	KeyFrameAnimator*  pKeyFrameAnimator = new KeyFrameAnimator(model->mSkeleton);

	pKeyFrameAnimator->Animate(model->GetModelMatrix(), deltaTime,  model->mAnimationMatrix, clipToAnimate);

	m_animationEventController.PurgeEndedClips(deltaTime);

	for (AnimationClip* clip : clips)
		clip->Update(deltaTime);

	delete pKeyFrameAnimator;

	if (clipSize > 1)
		delete clipToAnimate;


}

AnimationManager::~AnimationManager()
{

	for(auto iterator = m_animationSet.begin(); iterator != m_animationSet.end(); iterator++) {

		delete iterator->second;
		// iterator->first = key
		// iterator->second = value
		// Repeat if you also want to iterate through the second map.
	}
}



//std::map<string,AnimationClip*> AnimationManager::AnimationSet;

#endif // AnimationManager_h__
