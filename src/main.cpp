#include <sdk.hpp>
#include <Server/Components/Pawn/pawn.hpp>
#include <Server/Components/Pawn/Impl/pawn_natives.hpp>
#include <Server/Components/Pawn/Impl/pawn_impl.hpp>

#include "omp-logger.hpp"

class GreetComponent final : public IComponent, public PawnEventHandler
{
private:
    ICore* core_ = nullptr;
    IOmpLoggerComponent* ompLogger_ = nullptr;
    IPawnComponent* pawn_ = nullptr;

    inline static IOmpLog* logger_ = nullptr;
    inline static GreetComponent* instance_ = nullptr;

public:
    PROVIDE_UID(0x07619FCCC70AC039);

    ~GreetComponent()
    {
        if (pawn_)
        {
            pawn_->getEventDispatcher().removeEventHandler(this);
        }
    }

    StringView componentName() const override
    {
        return "Greet";
    }

    SemanticVersion componentVersion() const override
    {
        return SemanticVersion(0, 0, 1, 0);
    }

    void onLoad(ICore* c) override
    {
        core_ = c;
        core_->printLn("Greet component loaded.");
        setAmxLookups(core_);
    }

    void onInit(IComponentList* components) override
    {
        pawn_ = components->queryComponent<IPawnComponent>();
        if (pawn_)
        {
            setAmxFunctions(pawn_->getAmxFunctions());
            setAmxLookups(components);
            pawn_->getEventDispatcher().addEventHandler(this);
        }

        ompLogger_ = components->queryComponent<IOmpLoggerComponent>();
        if (ompLogger_ == nullptr)
        {
            core_->printLn("Greet component failed to load: omp-logger component not found");
            return;
        }

        OmpLoggerManager::Get().Initialize(ompLogger_);

        logger_ = OmpLoggerManager::Get().GetOmpLogger()->createLogger("greet", 0x00FF00, OmpLogger::ELogLevel::Info, true /* plugin */);
        if (logger_ != nullptr)
        {
            logger_->log(OmpLogger::ELogLevel::Info, "Successfully created logger.");
        }
    }

    void onAmxLoad(IPawnScript& script) override
    {
        AMX* amx = script.GetAMX();
        pawn_natives::AmxLoad(amx);
        OmpLoggerManager::Get().GetOmpLogger()->debugRegisterAMX(amx);
    }

    void onAmxUnload(IPawnScript& script) override
    {
        OmpLoggerManager::Get().GetOmpLogger()->debugEraseAMX(script.GetAMX());
    }

    void onReady() override
    {
    }

    void onFree(IComponent* component) override
    {
        if (component == pawn_)
        {
            pawn_ = nullptr;
            setAmxFunctions();
            setAmxLookups();
        }
        if (component == ompLogger_)
        {
            if (logger_ != nullptr)
            {
                ompLogger_->destroyLogger(logger_);
            }
            logger_ = nullptr;
        }
    }

    void free() override
    {
        delete this;
    }

    void reset() override
    {
    }

    static GreetComponent* Get()
    {
        if (instance_ == nullptr)
        {
            instance_ = new GreetComponent();
        }
        return instance_;
    }

    IOmpLog* GetLogger()
    {
        return logger_;
    }
};

COMPONENT_ENTRY_POINT()
{
    return GreetComponent::Get();
}

SCRIPT_API(Greet, bool())
{
    IOmpLog* logger = GreetComponent::Get()->GetLogger();
    if (logger == nullptr)
    {
        return false;
    }
    return logger->log(GetAMX(), OmpLogger::ELogLevel::Info, "Hi.");
}