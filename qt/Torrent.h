// This file Copyright © Mnemosyne LLC.
// It may be used under GPLv2 (SPDX: GPL-2.0-only), GPLv3 (SPDX: GPL-3.0-only),
// or any future license endorsed by Mnemosyne LLC.
// License text can be found in the licenses/ folder.

#pragma once

#include <bitset>
#include <cstddef> // size_t
#include <cstdint> // uint64_t
#include <ctime> // time_t
#include <optional>
#include <vector>

#include <QIcon>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QStringList>

#include <libtransmission/transmission.h>

#include <libtransmission/crypto-utils.h>
#include <libtransmission/quark.h>
#include <libtransmission/tr-macros.h>

#include "IconCache.h"
#include "Speed.h"

class QPixmap;

class Prefs;

extern "C"
{
    struct tr_variant;
}

struct Peer
{
    bool client_is_choked;
    bool client_is_interested;
    bool is_downloading_from;
    bool is_encrypted;
    bool is_incoming;
    bool is_uploading_to;
    bool peer_is_choked;
    bool peer_is_interested;
    QString address;
    QString client_name;
    QString flags;
    int port;
    Speed rate_to_client;
    Speed rate_to_peer;
    double progress;
};

using PeerList = std::vector<Peer>;

struct TrackerStat
{
    QPixmap getFavicon() const;

    bool has_announced;
    bool has_scraped;
    bool is_backup;
    bool last_announce_succeeded;
    bool last_announce_timed_out;
    bool last_scrape_succeeded;
    bool last_scrape_timed_out;
    int announce_state;
    int download_count;
    int id;
    int last_announce_peer_count;
    int last_announce_start_time;
    int last_announce_time;
    int last_scrape_start_time;
    int last_scrape_time;
    int leecher_count;
    int next_announce_time;
    int next_scrape_time;
    int scrape_state;
    int seeder_count;
    int tier;
    QString announce;
    QString last_announce_result;
    QString last_scrape_result;
    QString sitename;
};

using TrackerStatsList = std::vector<TrackerStat>;

struct TorrentFile
{
    bool wanted = true;
    int index = -1;
    int priority = 0;
    QString filename;
    uint64_t size = 0;
    uint64_t have = 0;
};

using FileList = std::vector<TorrentFile>;

class TorrentHash
{
private:
    tr_sha1_digest_t data_ = {};
    QString data_str_;

public:
    TorrentHash() = default;

    explicit TorrentHash(tr_sha1_digest_t const& data)
        : data_{ data }
    {
    }

    explicit TorrentHash(char const* str)
    {
        if (auto const hash = tr_sha1_from_string(str != nullptr ? str : ""); hash)
        {
            data_ = *hash;

            auto const tmpstr = tr_sha1_to_string(data_);
            data_str_ = QString::fromUtf8(std::data(tmpstr), std::size(tmpstr));
        }
    }

    explicit TorrentHash(QString const& str)
    {
        if (auto const hash = tr_sha1_from_string(str.toStdString()); hash)
        {
            data_ = *hash;

            auto const tmpstr = tr_sha1_to_string(data_);
            data_str_ = QString::fromUtf8(std::data(tmpstr), std::size(tmpstr));
        }
    }

    [[nodiscard]] TR_CONSTEXPR20 auto operator==(TorrentHash const& that) const
    {
        return data_ == that.data_;
    }

    [[nodiscard]] TR_CONSTEXPR20 auto operator!=(TorrentHash const& that) const
    {
        return !(*this == that);
    }

    [[nodiscard]] auto operator<(TorrentHash const& that) const
    {
        return data_ < that.data_;
    }

    [[nodiscard]] constexpr auto& toString() const noexcept
    {
        return data_str_;
    }
};

class Torrent : public QObject
{
    Q_OBJECT
    TR_DISABLE_COPY_MOVE(Torrent)

public:
    Torrent(Prefs const&, int id);

    [[nodiscard]] constexpr auto getBandwidthPriority() const noexcept
    {
        ID,
        UPLOAD_SPEED,
        DOWNLOAD_SPEED,
        DOWNLOAD_DIR,
        ACTIVITY,
        NAME,
        ERROR,
        ERROR_STRING,
        SIZE_WHEN_DONE,
        LEFT_UNTIL_DONE,
        HAVE_UNCHECKED,
        HAVE_VERIFIED,
        DESIRED_AVAILABLE,
        TOTAL_SIZE,
        PIECE_SIZE,
        PIECE_COUNT,
        PEERS_GETTING_FROM_US,
        PEERS_SENDING_TO_US,
        WEBSEEDS_SENDING_TO_US,
        PERCENT_DONE,
        METADATA_PERCENT_DONE,
        PERCENT_VERIFIED,
        DATE_ACTIVITY,
        DATE_ADDED,
        DATE_STARTED,
        DATE_CREATED,
        PEERS_CONNECTED,
        ETA,
        RATIO,
        DOWNLOADED_EVER,
        UPLOADED_EVER,
        FAILED_EVER,
        TRACKERS,
        HOSTS,
        TRACKERSTATS,
        MIME_ICON,
        SEED_RATIO_LIMIT,
        SEED_RATIO_MODE,
        SEED_IDLE_LIMIT,
        SEED_IDLE_MODE,
        DOWN_LIMIT,
        DOWN_LIMITED,
        UP_LIMIT,
        UP_LIMITED,
        SEQUENTIAL_DOWNLOAD,
        HONORS_SESSION_LIMITS,
        PEER_LIMIT,
        HASH_STRING,
        IS_FINISHED,
        IS_PRIVATE,
        IS_STALLED,
        COMMENT,
        CREATOR,
        MANUAL_ANNOUNCE_TIME,
        PEERS,
        BANDWIDTH_PRIORITY,
        QUEUE_POSITION,
        //
        PROPERTY_COUNT
    };

    typedef QList<tr_quark> KeyList;

  public:
    Torrent (const Prefs&, int id);
    virtual ~Torrent ();

    int getBandwidthPriority () const { return getInt (BANDWIDTH_PRIORITY); }
    int id () const { return getInt (ID); }
    QString name () const { return getString (NAME); }
    QString creator () const { return getString (CREATOR); }
    QString comment () const { return getString (COMMENT); }
    QString getPath () const { return getString (DOWNLOAD_DIR); }
    QString getError () const;
    QString hashString () const { return getString (HASH_STRING); }
    bool hasError () const { return !getError ().isEmpty (); }
    bool isDone () const { return getSize (LEFT_UNTIL_DONE) == 0; }
    bool isSeed () const { return haveVerified() >= totalSize(); }
    bool isPrivate () const { return getBool (IS_PRIVATE); }
    bool getSeedRatio (double& setme) const;
    uint64_t haveVerified () const { return getSize (HAVE_VERIFIED); }
    uint64_t haveUnverified () const { return getSize (HAVE_UNCHECKED); }
    uint64_t desiredAvailable () const { return getSize (DESIRED_AVAILABLE); }
    uint64_t haveTotal () const { return haveVerified () + haveUnverified(); }
    uint64_t totalSize () const { return getSize (TOTAL_SIZE); }
    uint64_t sizeWhenDone () const { return getSize (SIZE_WHEN_DONE); }
    uint64_t leftUntilDone () const { return getSize (LEFT_UNTIL_DONE); }
    uint64_t pieceSize () const { return getSize (PIECE_SIZE); }
    bool hasMetadata () const { return getDouble (METADATA_PERCENT_DONE) >= 1.0; }
    bool isMagnet () const { return magnetTorrent; }
    int  pieceCount () const { return getInt (PIECE_COUNT); }
    double ratio () const { return getDouble (RATIO); }
    double percentComplete () const { return haveTotal() / static_cast<double> (totalSize()); }
    double percentDone () const { return getDouble (PERCENT_DONE); }
    double metadataPercentDone () const { return getDouble (METADATA_PERCENT_DONE); }
    uint64_t downloadedEver () const { return getSize (DOWNLOADED_EVER); }
    uint64_t uploadedEver () const { return getSize (UPLOADED_EVER); }
    uint64_t failedEver () const { return getSize (FAILED_EVER); }
    int compareTracker (const Torrent&) const;
    int compareSeedRatio (const Torrent&) const;
    int compareRatio (const Torrent&) const;
    int compareETA (const Torrent&) const;
    bool hasETA () const { return getETA () >= 0; }
    int getETA () const { return getInt (ETA); }
    QDateTime lastActivity () const { return getDateTime (DATE_ACTIVITY); }
    QDateTime lastStarted () const { return getDateTime (DATE_STARTED); }
    QDateTime dateAdded () const { return getDateTime (DATE_ADDED); }
    QDateTime dateCreated () const { return getDateTime (DATE_CREATED); }
    QDateTime manualAnnounceTime () const { return getDateTime (MANUAL_ANNOUNCE_TIME); }
    bool canManualAnnounce () const { return isReadyToTransfer() && (manualAnnounceTime()<=QDateTime::currentDateTime()); }
    int peersWeAreDownloadingFrom () const { return getInt (PEERS_SENDING_TO_US); }
    int webseedsWeAreDownloadingFrom () const { return getInt (WEBSEEDS_SENDING_TO_US); }
    int peersWeAreUploadingTo () const { return getInt (PEERS_GETTING_FROM_US); }
    bool isUploading () const { return peersWeAreUploadingTo () > 0; }
    int connectedPeers () const { return getInt (PEERS_CONNECTED); }
    int connectedPeersAndWebseeds () const { return connectedPeers () + getInt (WEBSEEDS_SENDING_TO_US); }
    Speed downloadSpeed () const { return Speed::fromBps (getSize (DOWNLOAD_SPEED)); }
    Speed uploadSpeed () const { return Speed::fromBps (getSize (UPLOAD_SPEED)); }
    double getVerifyProgress () const { return getDouble (PERCENT_VERIFIED); }
    bool hasFileSubstring (const QString& substr) const;
    bool hasTrackerSubstring (const QString& substr) const;
    Speed uploadLimit () const { return Speed::fromKBps (getInt (UP_LIMIT)); }
    Speed downloadLimit () const { return Speed::fromKBps (getInt (DOWN_LIMIT)); }
    bool uploadIsLimited () const { return getBool (UP_LIMITED); }
    bool downloadIsLimited () const { return getBool (DOWN_LIMITED); }
    bool sequentialDownload () const { return getBool (SEQUENTIAL_DOWNLOAD); }
    bool honorsSessionLimits () const { return getBool (HONORS_SESSION_LIMITS); }
    int peerLimit () const { return getInt (PEER_LIMIT); }
    double seedRatioLimit () const { return getDouble (SEED_RATIO_LIMIT); }
    tr_ratiolimit seedRatioMode () const { return static_cast<tr_ratiolimit> (getInt (SEED_RATIO_MODE)); }
    int seedIdleLimit () const { return getInt (SEED_IDLE_LIMIT); }
    tr_idlelimit seedIdleMode () const { return static_cast<tr_idlelimit> (getInt (SEED_IDLE_MODE)); }
    TrackerStatsList trackerStats () const{ return myValues[TRACKERSTATS].value<TrackerStatsList>(); }
    QStringList trackers() const { return myValues[TRACKERS].value<QStringList>(); }
    QStringList hosts() const { return myValues[HOSTS].value<QStringList>(); }
    PeerList peers () const{ return myValues[PEERS].value<PeerList>(); }
    const FileList& files () const { return myFiles; }
    int queuePosition () const { return getInt (QUEUE_POSITION); }
    bool isStalled () const { return getBool (IS_STALLED); }

    QString activityString () const;
    tr_torrent_activity getActivity () const { return static_cast<tr_torrent_activity> (getInt (ACTIVITY)); }
    bool isFinished () const { return getBool (IS_FINISHED); }
    bool isPaused () const { return getActivity () == TR_STATUS_STOPPED; }
    bool isWaitingToVerify () const { return getActivity () == TR_STATUS_CHECK_WAIT; }
    bool isVerifying () const { return getActivity () == TR_STATUS_CHECK; }
    bool isDownloading () const { return getActivity () == TR_STATUS_DOWNLOAD; }
    bool isWaitingToDownload () const { return getActivity () == TR_STATUS_DOWNLOAD_WAIT; }
    bool isSeeding () const { return getActivity () == TR_STATUS_SEED; }
    bool isWaitingToSeed () const { return getActivity () == TR_STATUS_SEED_WAIT; }
    bool isReadyToTransfer () const { return getActivity()==TR_STATUS_DOWNLOAD || getActivity()==TR_STATUS_SEED; }
    bool isQueued () const { return isWaitingToDownload() || isWaitingToSeed(); }
    void notifyComplete () const;

    void update (tr_variant * dict);
    void setMagnet (bool magnet) { magnetTorrent = magnet; }

    QIcon getMimeTypeIcon () const { return getIcon (MIME_ICON); }

    static const KeyList& getInfoKeys ();
    static const KeyList& getStatKeys ();
    static const KeyList& getExtraStatKeys ();

  signals:
    void torrentChanged (int id);
    void torrentCompleted (int id);

  private:
    enum Group
    {
        return id_;
    }

    [[nodiscard]] constexpr auto const& name() const noexcept
    {
        return name_;
    }

    bool hasName() const
    {
        return !name_.isEmpty();
    }

    [[nodiscard]] constexpr auto const& creator() const noexcept
    {
        return creator_;
    }

    [[nodiscard]] constexpr auto const& comment() const noexcept
    {
        return comment_;
    }

    [[nodiscard]] constexpr auto const& getPath() const noexcept
    {
        return download_dir_;
    }

    QString getError() const;

    [[nodiscard]] constexpr auto const& trackerList() const noexcept
    {
        return tracker_list_;
    }

    [[nodiscard]] constexpr auto const& hash() const noexcept
    {
        return hash_;
    }

    [[nodiscard]] constexpr auto hasError() const noexcept
    {
        return error_ != TR_STAT_OK;
    }

    [[nodiscard]] constexpr auto leftUntilDone() const noexcept
    {
        return left_until_done_;
    }

    [[nodiscard]] constexpr auto isDone() const noexcept
    {
        return leftUntilDone() == 0;
    }

    [[nodiscard]] constexpr auto haveVerified() const noexcept
    {
        return have_verified_;
    }

    [[nodiscard]] constexpr auto totalSize() const noexcept
    {
        return total_size_;
    }

    [[nodiscard]] constexpr auto isSeed() const noexcept
    {
        return haveVerified() >= totalSize();
    }

    [[nodiscard]] constexpr auto isPrivate() const noexcept
    {
        return is_private_;
    }

    std::optional<double> getSeedRatioLimit() const;

    [[nodiscard]] constexpr auto haveUnverified() const noexcept
    {
        return have_unchecked_;
    }

    [[nodiscard]] constexpr auto desiredAvailable() const noexcept
    {
        return desired_available_;
    }

    [[nodiscard]] constexpr auto haveTotal() const noexcept
    {
        return haveVerified() + haveUnverified();
    }

    [[nodiscard]] constexpr auto sizeWhenDone() const noexcept
    {
        return size_when_done_;
    }

    [[nodiscard]] constexpr auto pieceSize() const noexcept
    {
        return piece_size_;
    }

    [[nodiscard]] constexpr auto metadataPercentDone() const noexcept
    {
        return metadata_percent_complete_;
    }

    [[nodiscard]] constexpr auto hasMetadata() const noexcept
    {
        return metadataPercentDone() >= 1.0;
    }

    [[nodiscard]] constexpr auto pieceCount() const noexcept
    {
        return piece_count_;
    }

    bool sequentialDownload() const
    {
        return getBool(SEQUENTIAL_DOWNLOAD);
    }

    [[nodiscard]] constexpr auto uploadedEver() const noexcept
    {
        return uploaded_ever_;
    }

    [[nodiscard]] constexpr auto ratio() const noexcept
    {
        auto const numerator = static_cast<double>(uploadedEver());
        auto const denominator = sizeWhenDone();
        return denominator > 0U ? numerator / denominator : double{};
    }

    [[nodiscard]] constexpr double percentComplete() const noexcept
    {
        return totalSize() != 0 ? haveTotal() / static_cast<double>(totalSize()) : 0;
    }

    [[nodiscard]] constexpr double percentDone() const noexcept
    {
        auto const l = leftUntilDone();
        auto const s = sizeWhenDone();
        return s ? static_cast<double>(s - l) / static_cast<double>(s) : 0.0;
    }

    [[nodiscard]] constexpr auto failedEver() const noexcept
    {
        return failed_ever_;
    }

    int compareSeedProgress(Torrent const&) const;
    int compareRatio(Torrent const&) const;
    int compareETA(Torrent const&) const;

    [[nodiscard]] constexpr auto getETA() const noexcept
    {
        return eta_;
    }

    [[nodiscard]] constexpr auto hasETA() const noexcept
    {
        return getETA() >= 0;
    }

    [[nodiscard]] constexpr auto lastActivity() const noexcept
    {
        return activity_date_;
    }

    [[nodiscard]] constexpr auto lastStarted() const noexcept
    {
        return start_date_;
    }

    [[nodiscard]] constexpr auto dateAdded() const noexcept
    {
        return added_date_;
    }

    [[nodiscard]] constexpr auto dateCreated() const noexcept
    {
        return date_created_;
    }

    [[nodiscard]] constexpr auto dateEdited() const noexcept
    {
        return edit_date_;
    }

    [[nodiscard]] constexpr auto manualAnnounceTime() const noexcept
    {
        return manual_announce_time_;
    }

    [[nodiscard]] constexpr auto peersWeAreDownloadingFrom() const noexcept
    {
        return peers_sending_to_us_;
    }

    [[nodiscard]] constexpr auto webseedsWeAreDownloadingFrom() const noexcept
    {
        return webseeds_sending_to_us_;
    }

    [[nodiscard]] constexpr auto peersWeAreUploadingTo() const noexcept
    {
        return peers_getting_from_us_;
    }

    [[nodiscard]] constexpr auto isUploading() const noexcept
    {
        return peersWeAreUploadingTo() > 0;
    }

    [[nodiscard]] constexpr auto connectedPeers() const noexcept
    {
        return peers_connected_;
    }

    [[nodiscard]] constexpr auto connectedPeersAndWebseeds() const noexcept
    {
        return connectedPeers() + webseedsWeAreDownloadingFrom();
    }

    [[nodiscard]] constexpr auto const& downloadSpeed() const noexcept
    {
        return download_speed_;
    }

    [[nodiscard]] constexpr auto const& uploadSpeed() const noexcept
    {
        return upload_speed_;
    }

    [[nodiscard]] constexpr auto getVerifyProgress() const noexcept
    {
        return recheck_progress_;
    }

    bool includesTracker(QString const& sitename) const;

    [[nodiscard]] constexpr auto const& labels() const noexcept
    {
        return labels_;
    }

    [[nodiscard]] constexpr auto const& sitenames() const noexcept
    {
        return sitenames_;
    }

    [[nodiscard]] constexpr auto uploadLimit() const
    {
        return Speed{ upload_limit_, Speed::Units::KByps };
    }

    [[nodiscard]] constexpr auto downloadLimit() const
    {
        return Speed{ download_limit_, Speed::Units::KByps };
    }

    [[nodiscard]] constexpr auto uploadIsLimited() const noexcept
    {
        return upload_limited_;
    }

    [[nodiscard]] constexpr auto downloadIsLimited() const noexcept
    {
        return download_limited_;
    }

    [[nodiscard]] constexpr auto honorsSessionLimits() const noexcept
    {
        return honors_session_limits_;
    }

    [[nodiscard]] constexpr auto peerLimit() const noexcept
    {
        return peer_limit_;
    }

    [[nodiscard]] constexpr auto seedRatioLimit() const noexcept
    {
        return seed_ratio_limit_;
    }

    [[nodiscard]] constexpr auto seedRatioMode() const noexcept
    {
        return static_cast<tr_ratiolimit>(seed_ratio_mode_);
    }

    [[nodiscard]] constexpr auto seedIdleLimit() const noexcept
    {
        return seed_idle_limit_;
    }

    [[nodiscard]] constexpr auto seedIdleMode() const noexcept
    {
        return static_cast<tr_idlelimit>(seed_idle_mode_);
    }

    [[nodiscard]] constexpr auto const& trackerStats() const noexcept
    {
        return tracker_stats_;
    }

    [[nodiscard]] constexpr auto const& peers() const noexcept
    {
        return peers_;
    }

    [[nodiscard]] constexpr auto const& files() const noexcept
    {
        return files_;
    }

    [[nodiscard]] auto constexpr queuePosition() const noexcept
    {
        return queue_position_;
    }

    [[nodiscard]] auto constexpr isStalled() const noexcept
    {
        return is_stalled_;
    }

    QString activityString() const;

    [[nodiscard]] auto constexpr getActivity() const noexcept
    {
        return static_cast<tr_torrent_activity>(status_);
    }

    [[nodiscard]] auto constexpr isFinished() const noexcept
    {
        return is_finished_;
    }

    [[nodiscard]] auto constexpr isPaused() const noexcept
    {
        return getActivity() == TR_STATUS_STOPPED;
    }

    [[nodiscard]] auto constexpr isWaitingToVerify() const noexcept
    {
        return getActivity() == TR_STATUS_CHECK_WAIT;
    }

    [[nodiscard]] auto constexpr isVerifying() const noexcept
    {
        return getActivity() == TR_STATUS_CHECK;
    }

    [[nodiscard]] auto constexpr isDownloading() const noexcept
    {
        return getActivity() == TR_STATUS_DOWNLOAD;
    }

    [[nodiscard]] auto constexpr isWaitingToDownload() const noexcept
    {
        return getActivity() == TR_STATUS_DOWNLOAD_WAIT;
    }

    [[nodiscard]] auto constexpr isSeeding() const noexcept
    {
        return getActivity() == TR_STATUS_SEED;
    }

    [[nodiscard]] auto constexpr isWaitingToSeed() const noexcept
    {
        return getActivity() == TR_STATUS_SEED_WAIT;
    }

    [[nodiscard]] auto constexpr isReadyToTransfer() const noexcept
    {
        return getActivity() == TR_STATUS_DOWNLOAD || getActivity() == TR_STATUS_SEED;
    }

    [[nodiscard]] auto constexpr isQueued() const noexcept
    {
        return isWaitingToDownload() || isWaitingToSeed();
    }

    [[nodiscard]] auto constexpr canManualAnnounceAt(time_t t) const noexcept
    {
        return isReadyToTransfer() && (manualAnnounceTime() <= t);
    }

    QIcon getMimeTypeIcon() const;

    enum Field
    {
        ACTIVITY_DATE,
        ADDED_DATE,
        BANDWIDTH_PRIORITY,
        COMMENT,
        CREATOR,
        DATE_CREATED,
        DESIRED_AVAILABLE,
        DOWNLOADED_EVER,
        DOWNLOAD_DIR,
        DOWNLOAD_LIMIT,
        DOWNLOAD_LIMITED,
        DOWNLOAD_SPEED,
        EDIT_DATE,
        TORRENT_ERROR,
        TORRENT_ERROR_STRING,
        ETA,
        FAILED_EVER,
        FILE_COUNT,
        FILES,
        HASH,
        HAVE_UNCHECKED,
        HAVE_VERIFIED,
        HONORS_SESSION_LIMITS,
        ICON,
        IS_FINISHED,
        IS_PRIVATE,
        IS_STALLED,
        LABELS,
        LEFT_UNTIL_DONE,
        MANUAL_ANNOUNCE_TIME,
        METADATA_PERCENT_COMPLETE,
        NAME,
        PEERS,
        PEERS_CONNECTED,
        PEERS_GETTING_FROM_US,
        PEERS_SENDING_TO_US,
        PEER_LIMIT,
        PERCENT_DONE,
        PIECE_COUNT,
        PIECE_SIZE,
        PRIMARY_MIME_TYPE,
        QUEUE_POSITION,
        RECHECK_PROGRESS,
        SEED_IDLE_LIMIT,
        SEED_IDLE_MODE,
        SEED_RATIO_LIMIT,
        SEED_RATIO_MODE,
        SIZE_WHEN_DONE,
        START_DATE,
        STATUS,
        TOTAL_SIZE,
        TRACKER_STATS,
        TRACKER_LIST,
        UPLOADED_EVER,
        UPLOAD_LIMIT,
        UPLOAD_LIMITED,
        UPLOAD_SPEED,
        WEBSEEDS_SENDING_TO_US,

        N_FIELDS
    };
    using fields_t = std::bitset<N_FIELDS>;

    fields_t update(tr_quark const* keys, tr_variant const* const* values, size_t n);

private:
    tr_torrent_id_t const id_;

    bool download_limited_ = {};
    bool honors_session_limits_ = {};
    bool is_finished_ = {};
    bool is_private_ = {};
    bool is_stalled_ = {};
    bool upload_limited_ = {};

    time_t activity_date_ = {};
    time_t added_date_ = {};
    time_t date_created_ = {};
    time_t edit_date_ = {};
    time_t manual_announce_time_ = {};
    time_t start_date_ = {};

    int bandwidth_priority_ = {};
    int error_ = {};
    int eta_ = {};
    int peer_limit_ = {};
    int peers_connected_ = {};
    int peers_getting_from_us_ = {};
    int peers_sending_to_us_ = {};
    int piece_count_ = {};
    int queue_position_ = {};
    int seed_idle_limit_ = {};
    int seed_idle_mode_ = {};
    int seed_ratio_mode_ = {};
    int status_ = {};
    int webseeds_sending_to_us_ = {};

    uint64_t desired_available_ = {};
    uint64_t download_limit_ = {};
    uint64_t downloaded_ever_ = {};
    uint64_t failed_ever_ = {};
    uint64_t file_count_ = {};
    uint64_t have_unchecked_ = {};
    uint64_t have_verified_ = {};
    uint64_t left_until_done_ = {};
    uint64_t piece_size_ = {};
    uint64_t size_when_done_ = {};
    uint64_t total_size_ = {};
    uint64_t upload_limit_ = {};
    uint64_t uploaded_ever_ = {};

    double metadata_percent_complete_ = {};
    double percent_done_ = {};
    double recheck_progress_ = {};
    double seed_ratio_limit_ = {};

    QString comment_;
    QString creator_;
    QString download_dir_;
    QString error_string_;
    QString name_;
    QString primary_mime_type_;
    QString tracker_list_;

    // mutable because it's a lazy lookup
    mutable QIcon icon_ = IconCache::get().fileIcon();

    PeerList peers_;
    FileList files_;

    QStringList labels_;
    std::vector<QString> sitenames_;
    TrackerStatsList tracker_stats_;

    Speed upload_speed_;
    Speed download_speed_;

    Prefs const& prefs_;

    TorrentHash hash_;
};

Q_DECLARE_METATYPE(Torrent const*)
