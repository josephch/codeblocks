#ifndef DIAGNOSTICS_CACHE_H
#define DIAGNOSTICS_CACHE_H

#include <unordered_map>
#include <mutex>
class DiagnosticsCache
{
public:
    void Insert(const wxString &filename, int line, wxString diagnostics)
    {
        std::scoped_lock < std::mutex > lock(m_mutex);
        auto &innermap = m_cache[filename];
        innermap[line] = std::move(diagnostics);
    }

    wxString Get(const wxString &filename, int line)
    {
        std::scoped_lock < std::mutex > lock(m_mutex);
        wxString ret;
        const auto &itr = m_cache.find(filename);
        if (itr != m_cache.end())
        {
            const auto &innermap = itr->second;
            const auto &inner_itr = innermap.find(line);
            if (inner_itr != innermap.end())
            {
                ret = inner_itr->second;
            }
            else
            {
                fprintf(stderr, "ClassBrowser::%s:%d: [%p] innermap of %s does not have line %d \n", __FUNCTION__,
                        __LINE__, this, filename.ToUTF8().data(), line);
            }
        }
        else
        {
            fprintf(stderr, "ClassBrowser::%s:%d: [%p] cache does not have information of file %s\n", __FUNCTION__,
                    __LINE__, this, filename.ToUTF8().data());
        }
        return ret;
    }

    void ClearFile(const wxString &filename)
    {
        std::scoped_lock < std::mutex > lock(m_mutex);
        wxString ret;
        const auto &itr = m_cache.find(filename);
        if (itr != m_cache.end())
        {
            m_cache.erase(itr);
        }
    }

private:
    std::unordered_map<wxString, std::unordered_map<int, wxString>> m_cache;
    std::mutex m_mutex;

};

#endif // DIAGNOSTICS_CACHE_H

