#include "LevelMeterComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
LevelMeterComponent::LevelMeterComponent()
{
    startTimerHz(30);
}

LevelMeterComponent::~LevelMeterComponent()
{
    stopTimer();
}

//==============================================================================
void LevelMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background
    MetallicLookAndFeel::drawRecessedPanel(g, bounds);
    
    // Meter area
    auto meterArea = bounds.reduced(4).toFloat();
    
    // Calculate meter height
    float meterHeight = meterArea.getHeight() * currentLevel;
    
    // Draw level bar
    auto levelRect = juce::Rectangle<float>(
        meterArea.getX(),
        meterArea.getBottom() - meterHeight,
        meterArea.getWidth(),
        meterHeight
    );
    
    // Color gradient: green -> yellow -> red
    juce::ColourGradient gradient(
        juce::Colours::red, levelRect.getX(), levelRect.getY(),
        juce::Colours::green, levelRect.getX(), levelRect.getBottom(),
        false
    );
    gradient.addColour(0.7, juce::Colours::yellow);
    
    g.setGradientFill(gradient);
    g.fillRect(levelRect);
    
    // Peak hold indicator
    if (peakLevel > 0.01f)
    {
        float peakY = meterArea.getBottom() - (meterArea.getHeight() * peakLevel);
        g.setColour(MetallicLookAndFeel::ACCENT_BRIGHT);
        g.drawLine(meterArea.getX(), peakY, meterArea.getRight(), peakY, 2.0f);
    }
    
    // dB markers
    g.setColour(MetallicLookAndFeel::TEXT_TERTIARY.withAlpha(0.5f));
    g.setFont(8.0f);
    
    auto drawMarker = [&](float db, const juce::String& label)
    {
        float level = juce::Decibels::decibelsToGain(db);
        float y = meterArea.getBottom() - (meterArea.getHeight() * level);
        g.drawText(label, meterArea.getRight() + 2, y - 4, 20, 8, 
                  juce::Justification::centredLeft);
    };
    
    drawMarker(0.0f, "0");
    drawMarker(-6.0f, "-6");
    drawMarker(-12.0f, "-12");
    drawMarker(-24.0f, "-24");
}

void LevelMeterComponent::resized()
{
}

//==============================================================================
void LevelMeterComponent::setLevel(float newLevel)
{
    currentLevel = juce::jlimit(0.0f, 1.0f, newLevel);
    
    // Update peak hold
    if (currentLevel > peakLevel)
    {
        peakLevel = currentLevel;
        peakHoldCounter = 60; // Hold for ~2 seconds at 30 fps
    }
    
    repaint();
}

void LevelMeterComponent::timerCallback()
{
    // Decay peak hold
    if (peakHoldCounter > 0)
    {
        --peakHoldCounter;
    }
    else if (peakLevel > 0.0f)
    {
        peakLevel *= 0.95f;
        if (peakLevel < 0.01f)
            peakLevel = 0.0f;
        repaint();
    }
}
