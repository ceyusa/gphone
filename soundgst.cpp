/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include <ptlib.h>
#include <ptlib/sound.h>
#include <ptclib/delaychan.h>

#include "mmbackend.h"

class PSoundChannelGst : public PSoundChannel
{
    PCLASSINFO(PSoundChannelGst, PSoundChannel);

public:
    PSoundChannelGst(MmBackend *backend) : m_backend(backend) {
        g_object_ref(m_backend);
    };
    ~PSoundChannelGst();
    PBoolean Open(const PString &device,
                  Directions dir,
                  unsigned numChannels,
                  unsigned sampleRate,
                  unsigned bitsPerSample);
    PBoolean Close();
    PBoolean Write(const void * buf, PINDEX len);
    PBoolean Read(void * buf, PINDEX len);
    PBoolean IsOpen() const;
    PBoolean SetBuffers(PINDEX size, PINDEX count) { return PTrue; }

    static PStringArray GetDeviceNames(PSoundChannel::Directions);

private:
    MmBackend *m_backend;
    PAdaptiveDelay m_pacing;
    unsigned m_sampleRate;
    unsigned m_sampleSize;
};

PSoundChannelGst::~PSoundChannelGst()
{
    Close();

    g_object_unref(m_backend);
}

PBoolean PSoundChannelGst::Open(const PString & device,
                                Directions dir,
                                unsigned numChannels,
                                unsigned sampleRate,
                                unsigned bitsPerSample)
{
    m_sampleRate = sampleRate;
    m_sampleSize = bitsPerSample;

    return mm_backend_audio_open(m_backend,
                                 (const char *) device,
                                 (MmBackendDirection) dir,
                                 numChannels,
                                 sampleRate);
}

PBoolean PSoundChannelGst::Close()
{
    mm_backend_audio_close(m_backend);

    return PTrue;
}

PBoolean PSoundChannelGst::IsOpen() const
{
    return mm_backend_audio_is_open(m_backend);
}

PBoolean PSoundChannelGst::Write(const void * buf, PINDEX len)
{
    size_t written;
    PBoolean ret;

    ret = mm_backend_audio_write(m_backend, buf, len, &written);
    lastWriteCount = ret ? written : 0;

    // As GStreamer is highly asynchronic and Opal expects
    // synchronicity, we should simulate the delay at writing.
    // The value of the delay is the same as the call
    // PCSSEP.SetSoundChannelBufferTime(depth)
    m_pacing.Delay(20);

    return ret;
}

PBoolean PSoundChannelGst::Read(void * buf, PINDEX len)
{
    size_t read;
    PBoolean ret;

    ret = mm_backend_audio_read(m_backend, buf, len, &read);
    lastReadCount = ret ? read : 0;

    return ret;
}

PStringArray PSoundChannelGst::GetDeviceNames(Directions dir)
{
    PStringArray devices;
    devices.AppendString("Gst");
    return devices;
}

class PSoundChannelPluginServiceDescriptorGst : public PDevicePluginServiceDescriptor
{
public:
    PSoundChannelPluginServiceDescriptorGst(MmBackend *backend)
        : m_backend (backend) { }

    virtual PObject *CreateInstance(int userData) const
        { return new PSoundChannelGst(m_backend); }

    virtual PStringArray GetDeviceNames(int userData) const
        {
            return PSoundChannelGst::GetDeviceNames(
                (PSoundChannel::Directions) userData
                );
        }

private:
    MmBackend *m_backend;
};

G_BEGIN_DECLS

/**
 * load_sound_channel: (skip)
 * @backend: a #MmBackend instance
 *
 * Loads the Gstreamer-based sound channel
 */
gboolean
load_sound_channel(MmBackend *backend)
{
    static PSoundChannelPluginServiceDescriptorGst GstSCDesc(backend);
    return PPluginManager::GetPluginManager().RegisterService("Gst",
                                                              "PSoundChannel",
                                                              &GstSCDesc);
}

G_END_DECLS
