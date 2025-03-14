/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "projectbuildtarget.h" // class's header file
    #include "cbproject.h"
    #include "manager.h"
    #include "projectmanager.h"
    #include "macrosmanager.h"
    #include "globals.h"
#endif



// class constructor
ProjectBuildTarget::ProjectBuildTarget(cbProject* parentProject)
    : m_Project(parentProject),
    m_FileArray(ProjectFile::CompareProjectFiles)
{
    m_BuildWithAll = false;
    m_CreateStaticLib = true;
    m_CreateDefFile = true;
    m_UseConsoleRunner = true;
    m_FileArray.Clear();
}

ProjectBuildTarget::ProjectBuildTarget(const ProjectBuildTarget &bt, cbProject* parentProject) :
    CompileTargetBase(bt),
    m_Project(parentProject),
    m_ExternalDeps(bt.m_ExternalDeps),
    m_AdditionalOutputFiles(bt.m_AdditionalOutputFiles),
    m_FileArray(ProjectFile::CompareProjectFiles),
    m_BuildWithAll(bt.m_BuildWithAll),
    m_CreateStaticLib(bt.m_CreateStaticLib),
    m_CreateDefFile(bt.m_CreateDefFile),
    m_UseConsoleRunner(bt.m_UseConsoleRunner)
{
}

// class destructor
ProjectBuildTarget::~ProjectBuildTarget()
{
}

cbProject* ProjectBuildTarget::GetParentProject()
{
    return m_Project;
}

const cbProject* ProjectBuildTarget::GetParentProject() const
{
    return m_Project;
}

wxString ProjectBuildTarget::GetFullTitle() const
{
    return m_Project->GetTitle() + _T(" - ") + GetTitle();
}

const wxString & ProjectBuildTarget::GetExternalDeps() const
{
    return m_ExternalDeps;
}

void ProjectBuildTarget::SetExternalDeps(const wxString& deps)
{
    if (m_ExternalDeps != deps)
    {
        m_ExternalDeps = deps;
        SetModified(true);
    }
}

const wxString & ProjectBuildTarget::GetAdditionalOutputFiles() const
{
    return m_AdditionalOutputFiles;
}

void ProjectBuildTarget::SetAdditionalOutputFiles(const wxString& files)
{
    if (m_AdditionalOutputFiles != files)
    {
        m_AdditionalOutputFiles = files;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetIncludeInTargetAll() const
{
    return m_BuildWithAll;
}

void ProjectBuildTarget::SetIncludeInTargetAll(bool buildIt)
{
    if (m_BuildWithAll != buildIt)
    {
        m_BuildWithAll = buildIt;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetCreateDefFile() const
{
    return m_CreateDefFile;
}

void ProjectBuildTarget::SetCreateDefFile(bool createIt)
{
    if (m_CreateDefFile != createIt)
    {
        m_CreateDefFile = createIt;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetCreateStaticLib() const
{
    return m_CreateStaticLib;
}

void ProjectBuildTarget::SetCreateStaticLib(bool createIt)
{
    if (m_CreateStaticLib != createIt)
    {
        m_CreateStaticLib = createIt;
        SetModified(true);
    }
}

bool ProjectBuildTarget::GetUseConsoleRunner() const
{
    if (GetTargetType() == ttConsoleOnly || GetRunHostApplicationInTerminal())
        return m_UseConsoleRunner;

    return false;
}

void ProjectBuildTarget::SetUseConsoleRunner(bool useIt)
{
    if (GetTargetType() == ttConsoleOnly && useIt != m_UseConsoleRunner)
    {
        m_UseConsoleRunner = useIt;
        SetModified(true);
    }
}

void ProjectBuildTarget::SetTargetType(TargetType pt)
{
    TargetType ttold = GetTargetType();
    CompileTargetBase::SetTargetType(pt);
    if (ttold != GetTargetType() && GetTargetType() == ttConsoleOnly)
        SetUseConsoleRunner(true); // by default, use console runner
}

ProjectFile* ProjectBuildTarget::GetFile(int index)
{
    if (m_FileArray.GetCount() == 0)
    {
        m_FileArray.Alloc(m_Files.size());
        for (FilesList::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
        {
            if (!*it)
                continue;
            m_FileArray.Add(*it);
        }
    }

    if (index < 0 || static_cast<size_t>(index) >= m_FileArray.GetCount())
        return nullptr;

    return m_FileArray.Item(index);
}

bool ProjectBuildTarget::RemoveFile(ProjectFile* pf)
{
    if (!pf)
        return false;
    m_Files.erase(pf);
    if (m_FileArray.GetCount() > 0)
        m_FileArray.Remove(pf);

    return true;
}
