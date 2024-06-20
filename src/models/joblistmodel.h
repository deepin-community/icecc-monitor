/*
    This file is part of Icecream.

    Copyright (c) 2012 Kevin Funk <kfunk@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef JOBLISTMODEL_H
#define JOBLISTMODEL_H

#include "job.h"

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QPointer>
#include <QVector>

class Monitor;
class QTimer;

class JobListModel
    : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Column
    {
        JobColumnID,
        JobColumnFilename,
        JobColumnClient,
        JobColumnServer,
        JobColumnState,
        JobColumnReal,
        JobColumnUser,
        JobColumnFaults,
        JobColumnSizeIn,
        JobColumnSizeOut,
        _JobColumnCount
    };

    enum JobType
    {
        AllJobs,
        LocalJobs,
        RemoteJobs
    };

    explicit JobListModel(QObject *parent = nullptr);

    Monitor *monitor() const;
    void setMonitor(Monitor *monitor);

    int expireDuration() const {
        return m_expireDuration;
    }

    void setExpireDuration(int duration) {
        m_expireDuration = duration;
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Job jobForIndex(const QModelIndex &index) const;
    QModelIndex indexForJob(const Job &job, int column);

    void setHostId(unsigned int hostId);
    unsigned int hostId() const { return m_hostId; }
    void setJobType(JobType type) { m_jobType = type; }
    JobType jobType() const { return m_jobType; }

private Q_SLOTS:
    void slotExpireFinishedJobs();

    void updateJob(const Job &job);
    void clear();

private:
    QVector<Job> m_jobs;

    void expireItem(const Job &job);
    void removeItem(const Job &job);
    void removeItemById(unsigned int jobId);

    QPointer<Monitor> m_monitor;

    /**
     * Number of parts (directories) of the file path which should be displayed.
     * -   < 0 for complete file path
     * -  == 0 for the pure file name without path
     * -   > 0 for only parts of the file path. If there are not enough parts
     *    the complete file path is displayed else .../partN/.../part1/fileName.
     * Default is 2.
     */
    int m_numberOfFilePathParts;

    /**
     * The number of seconds after which finished jobs should be expired.
     * -  < 0 never
     * - == 0 at once
     * -  > 0 after some seconds.
     * Default is -1.
     */
    int m_expireDuration;

    struct FinishedJob
    {
        explicit FinishedJob(uint _time = 0, uint _jobId = 0)
            : time(_time)
            , jobId(_jobId) {}
        uint time;
        uint jobId;
    };
    using FinishedJobs = QVector<JobListModel::FinishedJob>;

    /// List with job ids that are expired
    FinishedJobs m_finishedJobs;

    QTimer *m_expireTimer;
    JobType m_jobType;
    unsigned int m_hostId;
};

class JobListSortFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit JobListSortFilterProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif
